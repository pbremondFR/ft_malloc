/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 13:16:31 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/07 22:29:30 by pbremond         ###   ########.fr       */
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

static t_heap	*create_new_heap(size_t heap_size)
{
	const size_t len = ALIGN_TO(heap_size + sizeof(t_heap), getpagesize());

	t_heap *heap = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	heap->size = heap_size;
	// heap->next = NULL;	// Optional, mmap zero inits everything
	heap->chunks = (t_chunk*)ALIGN_MALLOC(heap + 1);	// Chunks start after heap header, correctly aligned
	// heap->chunks->prev_size = 0;
	heap->chunks->size = len - ((char*)heap->chunks - (char*)heap);
	heap->chunks->size |= FLAG_CHUNK_FREE;
	heap->chunks->next = NULL;
	return heap;
}

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
	++best_arenas->num_threads;
	best_arenas->tiny_heaps = create_new_heap(options.tiny_alloc_max_sz * TINY_HEAP_MIN_ELEM);
	best_arenas->small_heaps = create_new_heap(options.small_alloc_max_sz * SMALL_HEAP_MIN_ELEM);
	pthread_mutex_unlock(&best_arenas->mutex);
}

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

	if (size > PTRDIFF_MAX)
	{
		dbg_print("|Bad size, enomem and shit\n");
		errno = ENOMEM;
		return NULL;
	}
	else if (size <= TINY_ALLOC_MAX_SZ && false)
	{
		// ...
	}
	else if (size <= SMALL_ALLOC_MAX_SZ && false)
	{
		// ...
	}
	else
	{
		int		page_size		= getpagesize();
		size_t	header_size 	= ALIGN_MALLOC(sizeof(t_chunk));
		size_t	aligned_size	= ALIGN_TO(size + header_size, page_size);

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
