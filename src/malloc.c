/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 13:16:31 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/05 21:40:50 by pbremond         ###   ########.fr       */
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

t_malloc_internals	g_malloc_internals;

_Thread_local
t_malloc_arenas		*g_arenas;

typedef struct { const char *key; int *value; } t_kv_pair;

static void try_set_malloc_option(const char *env_var)
{
	t_kv_pair	options[] = {
		{"OPTIMISTIC_ARENA_ASSIGN",	&g_malloc_internals.optimistic_arena_assign},
		{"TINY_ALLOC_MAX",			&g_malloc_internals.tiny_alloc_max},
		{"SMALL_ALLOC_MAX",			&g_malloc_internals.small_alloc_max},
		{"CHECK_ERRORS",			&g_malloc_internals.check_errors},
	};

	for (unsigned int i = 0; i < SIZEOF_ARRAY(options); ++i)
	{
		if (ft_strnstr(env_var, options[i].key, ft_strlen(options[i].key)))
		{
			// Environment variables must have a value between 0 and 9
			const char *value_ptr = ft_strchr(env_var, '=') + 1;
			int value = ft_atoi(value_ptr);
			if (value != -1 || (value == -1 && *value_ptr == '-'))
				*options[i].value = value;
		}
	}
}

// TESTME: Might not work at all? I've seen some malloc calls BEFORE this was run
__attribute__((constructor))
static void	malloc_entrypoint(int argc, char *argv[], char *envp[])
{
	(void)argc, (void)argv;

	ft_putstr("Hello world, again\n");

	// Assign default values to all options
	g_malloc_internals.optimistic_arena_assign = 0;
	g_malloc_internals.tiny_alloc_max = TINY_ALLOC_MAX;
	g_malloc_internals.small_alloc_max = SMALL_ALLOC_MAX;
	g_malloc_internals.check_errors = 1;

	// Override default values with environment variables
	for (int i = 0; envp[i]; ++i)
		try_set_malloc_option(envp[i]);

	dbg_print("# Optimistic arena assign: %d\n", g_malloc_internals.optimistic_arena_assign);
	dbg_print("# Tiny alloc max: %d\n", g_malloc_internals.tiny_alloc_max);
	dbg_print("# Small alloc max: %d\n", g_malloc_internals.small_alloc_max);
	dbg_print("# Check errors: %d\n", g_malloc_internals.check_errors);

	for (unsigned int i = 0; i < SIZEOF_ARRAY(g_malloc_internals.arenas); ++i)
	{
		t_malloc_arenas *arenas = &g_malloc_internals.arenas[i];
		pthread_mutex_init(&arenas->mutex, NULL);
		arenas->num_threads = 0;
	}
}

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
			arenas->tiny_heaps = create_new_heap(g_malloc_internals.tiny_alloc_max * 100);
			arenas->small_heaps = create_new_heap(g_malloc_internals.small_alloc_max * 100);
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
	best_arenas->tiny_heaps = create_new_heap(g_malloc_internals.tiny_alloc_max * 100);
	best_arenas->small_heaps = create_new_heap(g_malloc_internals.small_alloc_max * 100);
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
	dbg_print("+-------------\n|IN MALLOC\n");
	// Completely unnecessary but I don't care, wanted to try it out once
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
		dbg_print("|Requested size: %zu\n", size);

		int		page_size = getpagesize();
		size_t	header_size = ALIGN_MALLOC(sizeof(t_chunk));
		size_t	aligned_size = ALIGN_TO(size + header_size, page_size);

		dbg_print("|header_size:    %zu\n", header_size);
		dbg_print("|aligned_size:   %zu\n", aligned_size);

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
		dbg_print("|Successfully mapped memory\n");

		void *ptr = chunk;
		ptr += header_size;

		dbg_print("|Pointer: %p\n", ptr);
		dbg_print("+-------------\n");

		return ptr;

		// return (char*)chunk + header_size;
	}
}
