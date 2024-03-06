/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 13:16:31 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/06 13:11:45 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include "ft_malloc_defines.h"
#include "ft_malloc_structs.h"
#include "ft_malloc_utils.h"
#include "libft.h"
#include <errno.h>
#include <sys/mman.h>
#include <stdbool.h>

t_malloc_internals	g_malloc_internals = {
	/* On this specific architecture, this is technically unnecessary. PTHREAD_MUTEX_INITIALIZER
	 * is equivalent to zero-initialization, and static variables as well as partial-initialization
	 * always zero-initialize the object. But I guess it's better to be safe/portable.
	*/
	.arenas = {
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
		{ 0, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
	},
	.loaded_options = false,
	.options = {0, TINY_ALLOC_MAX, SMALL_ALLOC_MAX, 0, 0},
};

_Thread_local
t_malloc_arenas		*g_arenas;

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

// # define FT_MANDATORY
/*
 * Assign the g_arenas pointer to the best available arenas collection, i.e.
 * the one with the least amount of threads using it.
 */
#ifndef FT_MANDATORY

static void	assign_arena_ptr()
{
	ft_putstr("Assigning arenas ptr\n");

	t_malloc_arenas	*best_arenas	= &g_malloc_internals.arenas[0];
	size_t			min_threads		= SIZE_MAX;

	for (unsigned int i = 0; i < sizeof(NUM_ARENAS); ++i)
	{
		t_malloc_arenas *arenas = &g_malloc_internals.arenas[i];
		pthread_mutex_lock(&arenas->mutex);
		if (arenas->num_threads == 0)
		{
			++arenas->num_threads;
			arenas->tiny_heaps = create_new_heap(g_malloc_internals.options.tiny_alloc_max * 100);
			arenas->small_heaps = create_new_heap(g_malloc_internals.options.small_alloc_max * 100);
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
	best_arenas->tiny_heaps = create_new_heap(g_malloc_internals.options.tiny_alloc_max * 100);
	best_arenas->small_heaps = create_new_heap(g_malloc_internals.options.small_alloc_max * 100);
	pthread_mutex_unlock(&best_arenas->mutex);
}

#else

static void	assign_arena_ptr()
{
	size_t zero = 0;
	size_t min_threads = SIZE_MAX;

	for (unsigned int i = 0; i < sizeof(NUM_ARENAS); ++i)
	{
		t_malloc_arenas *arenas = &g_malloc_internals.arenas[i];
		// If number of threads is zero, set it to one, set arenas pointers, and stop search
		if (atomic_compare_exchange_strong(&arenas->num_threads, &zero, 1))
		{
			pthread_mutex_lock(&arenas->mutex);
			// Initialize arena, heap, and chunks since we've never used it before
			arenas->tiny_heaps = create_new_heap(g_malloc_internals.tiny_alloc_max * 100);
			arenas->small_heaps = create_new_heap(g_malloc_internals.small_alloc_max * 100);
			g_arenas = arenas;
			pthread_mutex_unlock(&arenas->mutex);
			return;	// Early return
		}
		size_t num_threads_before = arenas->num_threads;
		if (!atomic_compare_exchange_strong(&arenas->num_threads, &num_threads_before, num_threads_before + 1))
		{
			i = 0;
			continue;
		}
	}
}

#endif

SHARED_LIB_EXPORT
void	*MALLOC(size_t size)
{
	ft_putstr("+-------------\n|IN MALLOC\n");
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
	else if (size <= TINY_ALLOC_MAX && false)
	{
		// ...
	}
	else if (size <= SMALL_ALLOC_MAX && false)
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
		// if (g_arenas->big_allocs == NULL)
		// 	g_arenas->big_allocs = chunk;
		// else
		// {
		// 	t_chunk *last = g_arenas->big_allocs;
		// 	while (last->next != NULL)
		// 		last = last->next;
		// 	last->next = chunk;
		// }
		return (char*)chunk + header_size;
	}
}
