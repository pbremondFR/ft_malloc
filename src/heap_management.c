/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_management.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 02:36:28 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/09 02:35:44 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_defines.h"
#include "ft_malloc_structs.h"
#include "ft_malloc_utils.h"
#include "ansi_color.h"
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>

#include <stdio.h>
#include "libft.h"

/*
 * Instanciate a new heap with given size, aligned to page size. Give it one big free chunk.
 */
t_heap	*create_new_heap(size_t size)
{
	const size_t aligned_size = ALIGN_TO(size + sizeof(t_heap), getpagesize());

	t_heap *heap = mmap(NULL, aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	heap->size = aligned_size;
	// heap->next = NULL;	// Optional, mmap zero inits everything
	heap->chunks = (t_chunk*)ALIGN_MALLOC(heap + 1);	// Chunks start after heap header, correctly aligned
	// heap->chunks->prev_size = 0;
	heap->chunks->size = aligned_size - ((char*)heap->chunks - (char*)heap);
	heap->chunks->size |= FLAG_CHUNK_FREE;
	heap->chunks->next = NULL;
	return heap;
}

/*
 * Insert a new t_heap in the linked list, in ascending order. If heap is directly adjacent
 * to the t_heap directly before or after (i.e. two pages are adjacent),
 * fuse these heaps together to avoid fragmentation
 */
void	insert_heap_in_list(t_heap **list, t_heap *new_heap)
{
	assert(*list != NULL);
	t_heap	*prev = NULL;
	size_t	page_size = getpagesize();

	// Do a sorted insert in the heap list
	if (*list > new_heap)
	{
		new_heap->next = *list;
		*list = new_heap;
	}
	else
	{
		t_heap *head = *list;
		while (head->next != NULL && new_heap > head->next)
			head = head->next;
		new_heap->next = head->next;
		head->next = new_heap;
		prev = head;
	}

	// char buf[256];
	// snprintf(buf, sizeof(buf), GRN
	// 	"page_size:      %#14lx\n"
	// 	"new_heap->size: %#14lx\n"
	// 	"prev:           %p\n"
	// 	"new_heap:       %p\n"
	// 	"new_heap->next: %p\n"RESET,
	// 	page_size, new_heap->size, prev, new_heap, new_heap->next);
	// ft_putstr(buf);
	// size_t prev_size = prev ? prev->size : 0;
	// snprintf(buf, sizeof(buf), BLU
	// 	"new + size:     %p\n"
	// 	"prev + size:    %p\n"RESET,
	// 	(void*)ALIGN_TO((char*)new_heap + new_heap->size, page_size),
	// 	(void*)ALIGN_TO((char*)prev + prev_size, page_size));
	// ft_putstr(buf);

	// TESTME: Optimization never kicks in on WSL so I can't test it, try it at school
	// Next is immediately adjacent, fuse the two t_heaps together
	if ((void*)ALIGN_TO((char*)new_heap + new_heap->size, page_size) == new_heap->next)
	{
		ft_putstr("Salut c'est David Lafarge\n");
		new_heap->size += new_heap->next->size;
		new_heap->next = new_heap->next->next;
	}
	// Previous is immediately adjacent, fuse them together
	if (prev && (void*)ALIGN_TO((char*)prev + prev->size, page_size) == new_heap)
	{
		ft_putstr("Salut c'est Lavid Dafarge\n");
		prev->size += new_heap->size;
		prev->next = new_heap->next;
	}
}

/*
 * Finds first free chunk in a given heap. Doesn't traverse heaps.
 */
static t_chunk	*find_first_free_chunk_in_heap(t_heap const *heap)
{
	t_chunk *chunk = heap->chunks;
	while (chunk != NULL && (char*)chunk < (char*)heap + heap->size)
	{
		if (chunk->size & FLAG_CHUNK_FREE)
			return chunk;
		else
			chunk = (t_chunk*)((char*)chunk + chunk_sz(chunk));
	}
	return NULL;
}

/*
 * Perform search for best chunk (that is, smallest possible size to avoid fragmentation)
 * in given list of heaps.
 */
t_chunk	*find_best_chunk_for_alloc(t_heap const *heaps, size_t req_size)
{
	t_chunk	*best_fit = NULL;
	req_size = ALIGN_MALLOC(req_size);

	for (const t_heap *head = heaps; head != NULL; head = head->next)
	{
		t_chunk *chunk = find_first_free_chunk_in_heap(head);
		while (chunk != NULL && (char*)chunk < (char*)head + head->size)
		{
			if (chunk->size & FLAG_CHUNK_FREE
				&& chunk_alloc_sz(chunk) >= req_size
				&& (!best_fit || chunk_sz(chunk) < chunk_sz(best_fit)))
			{
				best_fit = chunk;
			}
			chunk = chunk->next;
		}
	}
	return best_fit;
}

static void	try_shrink_chunk_for_requested_size(t_chunk *chunk, size_t req_size)
{
	size_t new_size = ALIGN_MALLOC(req_size + sizeof(t_chunk));
	// If new chunk size doesn't leave enough the minimum amount of space for next chunk,
	// do nothing
	if (chunk_sz(chunk) - new_size < ALIGN_MALLOC(sizeof(t_chunk) + MALLOC_ALIGNMENT))
		return;

	// Else, divide chunk
	t_chunk *new_next_chunk = (void*)chunk + new_size;
	new_next_chunk->next = chunk->next;
	new_next_chunk->size = (chunk_sz(chunk) - new_size) | FLAG_CHUNK_FREE;
	if (!new_next_chunk->next)
		ft_putstr(BRED"No next chunk??"RESET"\n");
	else
		ft_putstr(BGRN"Yes next chunk"RESET"\n");
	if (new_next_chunk->next && !(new_next_chunk->size & FLAG_CHUNK_PREV_FREE))
		ft_putstr(REDB"We fucked up"RESET"\n");
		// assert(new_next_chunk->next->size & FLAG_CHUNK_PREV_FREE);
	size_t *new_trailing_sz_tag = (void*)new_next_chunk + chunk_sz(new_next_chunk) - sizeof(size_t);
	*new_trailing_sz_tag = chunk_sz(new_next_chunk);

	chunk->size = new_size | (chunk->size & ~CHUNK_SIZE_MASK);	// Keep the flags
	chunk->next = new_next_chunk;
}

t_chunk	*alloc_chunk_from_heaps(t_heap **heap_lst, size_t req_size, size_t new_heap_min_sz)
{
	t_chunk *selected_chunk = find_best_chunk_for_alloc(*heap_lst, req_size);
	if (selected_chunk == NULL)
	{
		ft_putstr(RED"No good free chunk, inserting new heap"RESET"\n");
		t_heap *new_heap = create_new_heap(new_heap_min_sz);
		if (!new_heap)
			return NULL;
		insert_heap_in_list(heap_lst, new_heap);
		selected_chunk = find_best_chunk_for_alloc(*heap_lst, req_size);
	}
	try_shrink_chunk_for_requested_size(selected_chunk, req_size);
	selected_chunk->size &= ~FLAG_CHUNK_FREE;
	return selected_chunk;
}
