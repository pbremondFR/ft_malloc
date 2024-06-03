/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 16:32:51 by pbremond          #+#    #+#             */
/*   Updated: 2024/06/03 18:11:10 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include "ft_malloc_structs.h"
#include "ft_malloc_defines.h"
#include "ft_malloc_utils.h"
#include <sys/mman.h>
#include <errno.h>

#include <stdio.h>
#include <unistd.h>
#include "libft.h"
#include "ansi_color.h"

static void	remove_large_chunk_from_list(t_chunk *chunk)
{
	t_chunk	*prev = g_malloc_internals.arenas.big_allocs;
	while (prev != chunk && prev->next != NULL && prev->next != chunk)
		prev = prev->next;
	if (prev == chunk)
		g_malloc_internals.arenas.big_allocs = chunk->next;
	else
		prev->next = chunk->next;
}

static t_chunk	*free_chunk(t_chunk *chunk, t_malloc_options const *opt)
{
	if (opt->check_errors && chunk_alloc_sz(chunk) > SMALL_ALLOC_MAX_SZ)
	{
		malloc_error("Free'd chunk has incoherent alloc size, possible heap corruption\n",
			opt->check_errors & 0x2);
		return NULL;
	}

	// Collapse next chunk with currently free'd chunk
	if (chunk->next && chunk->next->size & FLAG_CHUNK_FREE)
	{
		size_t flags = chunk->size & ~CHUNK_SIZE_MASK;
		chunk->size = (chunk_sz(chunk) + chunk_sz(chunk->next)) | flags;
		chunk->next = chunk->next->next;
	}

	// Collapse previous chunk if it's free
	if (chunk->size & FLAG_CHUNK_PREV_FREE)
	{
		const size_t *p_prev_size = (void*)chunk - sizeof(size_t);
		const size_t prev_size = *p_prev_size;
		if (opt->check_errors && prev_size == 0)
		{	// Don't immediately fail, just give up on coalescing the previous chunk
			malloc_error("Chunk's previous size data is corrupted\n", opt->check_errors & 0x2);
		}
		else
		{
			t_chunk	*prev = (void*)chunk - prev_size;
			prev->size = (chunk_sz(chunk) + chunk_sz(prev));
			prev->next = chunk->next;
			chunk = prev;
		}
	}

	chunk->size |= FLAG_CHUNK_FREE;
	if (chunk->next)	// Set tag to indicate that previous chunk is free
		chunk->next->size |= FLAG_CHUNK_PREV_FREE;

	size_t *trailing_size_tag = (void*)chunk + chunk_sz(chunk) - sizeof(size_t);
	*trailing_size_tag = chunk_sz(chunk);
	return chunk;
}

SHARED_LIB_EXPORT
void	FREE(void *ptr)
{
	// Replace the above line with this for debug, move it below when I'm done
	if (ptr == NULL)
		return;
	const int preserved_errno = errno;
	const t_malloc_options opt = g_malloc_internals.options;
	t_chunk *chunk = ptr - ALIGN_MALLOC(sizeof(t_chunk));

	pthread_mutex_lock(&g_malloc_internals.arenas.mutex);
	if (opt.check_errors && chunk->size & FLAG_CHUNK_FREE)
	{
		malloc_error("Double free\n", opt.check_errors & 0x2);
		goto unlock_and_exit;
	}
	if (chunk->size & FLAG_CHUNK_MMAPPED)
	{
		remove_large_chunk_from_list(chunk);
		if (munmap(chunk, chunk_sz(chunk)) != 0)
			malloc_error("munmap() call failed, likely due to heap corruption\n", opt.check_errors & 0x2);
	}
	else
	{
		size_t alloc_sz = chunk_alloc_sz(chunk);
		t_chunk *freed = free_chunk(chunk, &opt);
		if (likely(freed))
		{
			t_heap *heap_head = alloc_sz <= (size_t)opt.tiny_alloc_max_sz
				? g_malloc_internals.arenas.tiny_heaps
				: g_malloc_internals.arenas.small_heaps;
			size_t empty_heap_chunk_sz = heap_head->size
				- ALIGN_MALLOC(sizeof(t_heap));
			if (chunk_sz(freed) == empty_heap_chunk_sz && heap_head->next)
			{
				t_heap *target = (void*)freed - ALIGN_MALLOC(sizeof(t_heap));
				remove_heap_from_list(heap_head, target);
				if (munmap(target, target->size) != 0) {
					malloc_error("Failed to unmap arena\n", opt.check_errors & 0x2);
				}
			}
		}
	}

unlock_and_exit:
	pthread_mutex_unlock(&g_malloc_internals.arenas.mutex);
	errno = preserved_errno;
	return;
}
