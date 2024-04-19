/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 16:32:51 by pbremond          #+#    #+#             */
/*   Updated: 2024/04/19 17:47:22 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include "ft_malloc_structs.h"
#include "ft_malloc_defines.h"
#include "ft_malloc_utils.h"
#include <sys/mman.h>

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

static void	free_chunk(t_chunk *chunk)
{
	if (chunk_alloc_sz(chunk) > SMALL_ALLOC_MAX_SZ)
	{
		tarace(REDB"Fucking bad chunk alloc size (%zu), corrupted or something???"RESET"\n",
			chunk_alloc_sz(chunk));
		return;
	}

	// Collapse next chunk with currently free'd chunk
	// dbg_print(BYEL"Freeing chunk"RESET"\n");
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
		if (prev_size == 0)
		{
			tarace(REDB"ptn zbi prev size is %zu"RESET"\n", prev_size);
			abort();
		}
		t_chunk	*prev = (void*)chunk - prev_size;
		// size_t flags = prev->size & ~CHUNK_SIZE_MASK;
		prev->size = (chunk_sz(chunk) + chunk_sz(prev));
		prev->next = chunk->next;
		chunk = prev;
	}

	chunk->size |= FLAG_CHUNK_FREE;
	if (chunk->next)	// Set tag to indicate that previous chunk is free
		chunk->next->size |= FLAG_CHUNK_PREV_FREE;

	size_t *trailing_size_tag = (void*)chunk + chunk_sz(chunk) - sizeof(size_t);
	*trailing_size_tag = chunk_sz(chunk);
}

// FIXME: MT arenas make this tricky, because nothing stops a thread from
// allocating memory, passing the pointer to another thread and having
// another thread freeing it. Either definitely stop from that multiple
// arenas bullshit or identify which heap the chunk comes from before
// freeing it.
SHARED_LIB_EXPORT
void	FREE(void *ptr)
{
	// Replace the above line with this for debug, move it below when I'm done
	if (ptr == NULL)
		return;
	pthread_mutex_lock(&g_malloc_internals.arenas.mutex);
	t_chunk *chunk = ptr - ALIGN_MALLOC(sizeof(t_chunk));
	if (chunk->size & FLAG_CHUNK_FREE)
	{
		tarace(REDB"CORRUPTED, DOUBLE FREE %p\n"RESET, chunk);
		abort();
	}
	if (chunk->size & FLAG_CHUNK_MMAPPED)
	{
		remove_large_chunk_from_list(chunk);
		if (munmap(chunk, chunk_sz(chunk)) != 0) {
			tarace(BHRED"OH FUCK\n"RESET);
		}
	}
	else
	{
		free_chunk(chunk);
	}
	pthread_mutex_unlock(&g_malloc_internals.arenas.mutex);
}
