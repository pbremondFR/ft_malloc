/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_management.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 02:36:28 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/08 02:47:28 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_defines.h"
#include "ft_malloc_structs.h"
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>

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

	// Next is immediately adjacent, fuse the two t_heaps together
	if ((void*)ALIGN_TO((char*)new_heap + new_heap->size, page_size) == new_heap->next)
	{
		new_heap->size += new_heap->next->size;
		new_heap->next = new_heap->next->next;
	}
	// Previous is immediately adjacent, fuse them together
	if ((void*)ALIGN_TO((char*)prev + prev->size, page_size) == new_heap)
	{
		prev->size += new_heap->size;
		prev->next = new_heap->next;
	}
}
