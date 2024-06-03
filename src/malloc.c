/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 13:16:31 by pbremond          #+#    #+#             */
/*   Updated: 2024/06/03 18:07:29 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include "ft_malloc_defines.h"
#include "ft_malloc_structs.h"
#include "ft_malloc_utils.h"
#include "libft.h"
#include "ansi_color.h"
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <stdbool.h>

static bool	init_arenas(t_malloc_options const *options)
{
	pthread_mutex_lock(&g_malloc_internals.arenas.mutex);
	// Other thread already init while we were waiting for mutex, skip it
	if (g_malloc_internals.arenas.is_init == true)
	{
		pthread_mutex_unlock(&g_malloc_internals.arenas.mutex);
		return true;
	}

	t_heap *tiny_heap = create_new_heap(
		(options->tiny_alloc_max_sz + sizeof(t_chunk)) * TINY_HEAP_MIN_ELEM);
	t_heap *small_heap = create_new_heap(
		(options->small_alloc_max_sz + sizeof(t_chunk)) * SMALL_HEAP_MIN_ELEM);

	if (!tiny_heap || !small_heap)
	{
#ifdef FT_BONUS
		char msg[128] = "Failed to initialize arenas with mmap: ";
		ft_strlcpy(msg, strerror(errno), sizeof(msg));
		malloc_error(msg, options->check_errors & 0x2);
#endif
		if (tiny_heap)
			munmap(tiny_heap, tiny_heap->size);
		if (small_heap)
			munmap(small_heap, small_heap->size);
		pthread_mutex_unlock(&g_malloc_internals.arenas.mutex);
		return false;
	}

	g_malloc_internals.arenas.tiny_heaps = tiny_heap;
	g_malloc_internals.arenas.small_heaps = small_heap;
	g_malloc_internals.arenas.is_init = true;
	pthread_mutex_unlock(&g_malloc_internals.arenas.mutex);
	return true;
}

/*
 * Do a sorted insert of given chunk in given chunks linked list. Sort is based on chunk address.
 */
static void	insert_chunk_in_list(t_chunk **list, t_chunk *chunk)
{
	if (*list == NULL || *list > chunk)
	{
		chunk->next = *list;
		*list = chunk;
		return;
	}
	t_chunk *head = *list;
	while (head->next != NULL && chunk > head->next)
		head = head->next;
	chunk->next = head->next;
	head->next = chunk;
}

static void	*malloc_tiny_or_small(t_heap **heaps, size_t req_size, size_t min_heap_size)
{
	pthread_mutex_lock(&g_malloc_internals.arenas.mutex);
	t_chunk *chunk = alloc_chunk_from_heaps(heaps, req_size, min_heap_size);
	pthread_mutex_unlock(&g_malloc_internals.arenas.mutex);
	if (!chunk)
	{
		errno = ENOMEM;
		return NULL;
	}
	else
		return (char*)chunk + ALIGN_MALLOC(sizeof(t_chunk));
}

SHARED_LIB_EXPORT
void	*MALLOC(size_t size)
{
	if (unlikely(g_malloc_internals.loaded_options == false))
		malloc_load_options();
	t_malloc_options options = g_malloc_internals.options;
	if (unlikely(g_malloc_internals.arenas.is_init == false && !init_arenas(&options)))
	{
		errno = ENOMEM;
		return NULL;
	}

	if (size > PTRDIFF_MAX)
	{
		errno = ENOMEM;
		return NULL;
	}
	else if (size == 0)
	{
		return NULL;
	}
	else if (size <= (size_t)options.tiny_alloc_max_sz)
	{
		return malloc_tiny_or_small(&g_malloc_internals.arenas.tiny_heaps,
			size, (options.tiny_alloc_max_sz + sizeof(t_chunk)) * TINY_HEAP_MIN_ELEM);
	}
	else if (size <= (size_t)options.small_alloc_max_sz)
	{
		return malloc_tiny_or_small(&g_malloc_internals.arenas.small_heaps,
			size, (options.small_alloc_max_sz + sizeof(t_chunk)) * SMALL_HEAP_MIN_ELEM);
	}
	else
	{
		size_t	header_size 	= ALIGN_MALLOC(sizeof(t_chunk));
		size_t	aligned_size	= ALIGN_MALLOC(size + header_size);

		t_chunk *chunk = mmap(NULL, aligned_size, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (chunk == MAP_FAILED)
		{
			errno = ENOMEM;
			return NULL;
		}
		chunk->size = aligned_size | FLAG_CHUNK_MMAPPED;
		chunk->next = NULL;
		/* Append chunk to list of big allocs. Required by subject for debugging functions */
		pthread_mutex_lock(&g_malloc_internals.arenas.mutex);
		insert_chunk_in_list(&g_malloc_internals.arenas.big_allocs, chunk);
		pthread_mutex_unlock(&g_malloc_internals.arenas.mutex);
		void *mem = (void*)chunk + header_size;
		return mem;
	}
}
