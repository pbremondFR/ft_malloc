/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 13:16:31 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/08 22:28:48 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include "ft_malloc_defines.h"
#include "ft_malloc_structs.h"
#include "ft_malloc_utils.h"
#include "libft.h"
#include "ansi_color.h"
#include <errno.h>
#include <sys/mman.h>
#include <stdbool.h>

/*
 * Assign the g_arenas pointer to the best available arenas collection, i.e.
 * the one with the least amount of threads using it.
 */
static void	assign_arena_ptr()
{
	ft_putstr(CYN"Assigning arenas ptr"RESET"\n");

	t_malloc_arenas		*best_arenas	= &g_malloc_internals.arenas[0];
	size_t				min_threads		= SIZE_MAX;
	t_malloc_options	options			= g_malloc_internals.options;

	for (unsigned int i = 0; i < NUM_ARENAS; ++i)
	{
		t_malloc_arenas *arenas = &g_malloc_internals.arenas[i];
		pthread_mutex_lock(&arenas->mutex);
		if (arenas->num_threads == 0)
		{
			++arenas->num_threads;
			arenas->tiny_heaps = create_new_heap(options.tiny_alloc_max_sz * TINY_HEAP_MIN_ELEM);
			arenas->small_heaps = create_new_heap(options.small_alloc_max_sz * SMALL_HEAP_MIN_ELEM);
			g_arenas = arenas;
			pthread_mutex_unlock(&arenas->mutex);
			return;
		}
		else
		{
			if (arenas->num_threads < min_threads)
			{
				min_threads = arenas->num_threads;
				best_arenas = arenas;
			}
			pthread_mutex_unlock(&arenas->mutex);
		}
	}
	pthread_mutex_lock(&best_arenas->mutex);
	g_arenas = best_arenas;
	if (++best_arenas->num_threads == 1)	// First thread to obtain this arena
	{
		best_arenas->tiny_heaps = create_new_heap(options.tiny_alloc_max_sz * TINY_HEAP_MIN_ELEM);
		best_arenas->small_heaps = create_new_heap(options.small_alloc_max_sz * SMALL_HEAP_MIN_ELEM);
	}
	pthread_mutex_unlock(&best_arenas->mutex);
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

SHARED_LIB_EXPORT
void	*MALLOC(size_t size)
{
	// ft_putstr(GRN"+-------------\n|IN MALLOC"RESET"\n");
	if (unlikely(g_malloc_internals.loaded_options == false))
		malloc_load_options();
	if (unlikely(g_arenas == NULL))
		assign_arena_ptr();

	t_malloc_options options = g_malloc_internals.options;
	if (size > PTRDIFF_MAX)
	{
		dbg_print("|Bad size, enomem and shit\n");
		errno = ENOMEM;
		return NULL;
	}
	else if (size <= (size_t)options.tiny_alloc_max_sz)
	{
		pthread_mutex_lock(&g_arenas->mutex);
		t_chunk *chunk = alloc_chunk_from_heaps(&g_arenas->tiny_heaps, size,
			options.tiny_alloc_max_sz * TINY_HEAP_MIN_ELEM);
		pthread_mutex_unlock(&g_arenas->mutex);
		if (!chunk)
		{
			errno = ENOMEM;
			return NULL;
		}
		else
			return (char*)chunk + ALIGN_MALLOC(sizeof(t_chunk));
	}
	else if (size <= (size_t)options.small_alloc_max_sz)
	{
		pthread_mutex_lock(&g_arenas->mutex);
		t_chunk *chunk = alloc_chunk_from_heaps(&g_arenas->small_heaps, size,
			options.small_alloc_max_sz * SMALL_HEAP_MIN_ELEM);
		pthread_mutex_unlock(&g_arenas->mutex);
		if (!chunk)
		{
			errno = ENOMEM;
			return NULL;
		}
		else
			return (char*)chunk + ALIGN_MALLOC(sizeof(t_chunk));
	}
	else
	{
		size_t	header_size 	= ALIGN_MALLOC(sizeof(t_chunk));
		size_t	aligned_size	= ALIGN_MALLOC(size + header_size);

		t_chunk *chunk = mmap(NULL, aligned_size, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (chunk == MAP_FAILED)
		{
			dbg_print("|BIG ASS ERROR\n");
			errno = ENOMEM;
			return NULL;
		}
		// chunk->prev_size = 0;
		chunk->size = aligned_size;
		chunk->size |= FLAG_CHUNK_MMAPPED;
		chunk->next = NULL;
		/* Append chunk to list of big allocs. Required by subject for debugging functions */
		pthread_mutex_unlock(&g_arenas->mutex);
		insert_chunk_in_list(&g_arenas->big_allocs, chunk);
		pthread_mutex_unlock(&g_arenas->mutex);
		// ft_putstr(GRN"|OUT OF MALLOC\n+-------------"RESET"\n");
		return (char*)chunk + header_size;
	}
}
