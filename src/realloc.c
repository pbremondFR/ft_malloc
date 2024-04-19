/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 17:50:53 by pbremond          #+#    #+#             */
/*   Updated: 2024/04/19 17:53:09 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include "ft_malloc_structs.h"
#include "ft_malloc_defines.h"
#include "ft_malloc_utils.h"
#include "libft.h"
#include <errno.h>

static t_chunk	*get_coalesced_chunk(size_t prev_sz, size_t next_sz, size_t req_size,
	t_chunk *chunk)
{
	// Only next chunk is enough
	if (chunk_alloc_sz(chunk) + next_sz >= req_size)
	{
		chunk->size += next_sz;
		chunk->next = chunk->next->next;
	}
	else
	{
		// Coalesce the previous chunk with this one and copy data
		t_chunk *prev = (void*)chunk - prev_sz;
		prev->size += chunk_sz(chunk);
		prev->next = chunk->next;
		ft_memmove(
			(void*)prev + ALIGN_MALLOC(sizeof(t_chunk)),
			(void*)chunk + ALIGN_MALLOC(sizeof(t_chunk)),
			chunk_alloc_sz(chunk)
		);
		chunk = prev;
		// Still not enough room coalescing prev, also coalesce next one
		if (chunk_alloc_sz(chunk) < req_size)
		{
			chunk->size += chunk_sz(chunk->next);
			chunk->next = chunk->next->next;
		}
	}
	chunk->size &= ~FLAG_CHUNK_FREE;
	if (chunk->next)
		chunk->next->size &= ~FLAG_CHUNK_PREV_FREE;
	return chunk;
}

static void	*copy_chunk_in_new_alloc(void *ptr, const t_chunk *chunk, size_t req_size,
	bool *released_mutex)
{
	pthread_mutex_unlock(&g_malloc_internals.arenas.mutex);
	*released_mutex = true;
	void *newptr = MALLOC(req_size);
	// From manual:
	// "If realloc() fails, the original block is left untouched; it is not freed or moved"
	if (newptr != NULL)
	{
		size_t copy_len = req_size < chunk_alloc_sz(chunk) ? req_size : chunk_alloc_sz(chunk);
		ft_memcpy(newptr, ptr, copy_len);
		FREE(ptr);
	}
	return newptr;
}

static void	*enlarge_or_alloc_new(void *ptr, t_chunk *chunk, size_t req_size,
	bool *released_mutex)
{
	void *retval = NULL;
	size_t prev_size = 0, next_size = 0;

	// Get sizes of previous and next chunks to see if coalescing is possible
	if (chunk->next && chunk->next->size & FLAG_CHUNK_FREE)
		next_size = chunk_sz(chunk->next);
	if (chunk->size & FLAG_CHUNK_PREV_FREE)
	{
		const size_t *p_prev_size = (void*)chunk - sizeof(size_t);
		prev_size = *p_prev_size;
	}

	// There is enough space, coalesce next, prev, or both
	if (chunk_alloc_sz(chunk) + prev_size + next_size >= req_size)
	{
		t_chunk *new_chunk = get_coalesced_chunk(prev_size, next_size, req_size, chunk);
		try_shrink_chunk_for_requested_size(new_chunk, req_size);
		retval = (void*)new_chunk + ALIGN_MALLOC(sizeof(t_chunk));
	}
	else	// Cannot coalesce chunk, just malloc copy and free
	{
		retval = copy_chunk_in_new_alloc(ptr, chunk, req_size, released_mutex);
	}
	return retval;

}

static bool	new_size_ok_for_arena(size_t alloc_sz, size_t req_size)
{
	size_t new_alloc_sz = ALIGN_MALLOC(req_size);
	if (alloc_sz <= TINY_ALLOC_MAX_SZ)
	{
		return new_alloc_sz <= TINY_ALLOC_MAX_SZ;
	}
	else if (alloc_sz <= SMALL_ALLOC_MAX_SZ)
	{
		return new_alloc_sz > TINY_ALLOC_MAX_SZ
			&& new_alloc_sz <= SMALL_ALLOC_MAX_SZ;
	}
	else
	{
		return new_alloc_sz > SMALL_ALLOC_MAX_SZ;
	}
}

SHARED_LIB_EXPORT
void	*REALLOC(void *ptr, size_t req_size)
{
	if (ptr == NULL)
		return MALLOC(req_size);
	else if (req_size == 0)
	{
		FREE(ptr);
		return NULL;
	}

	t_chunk *chunk	= ptr - ALIGN_MALLOC(sizeof(t_chunk));
	size_t	size	= ALIGN_MALLOC(req_size);
	void	*retval	= NULL;

	pthread_mutex_lock(&g_malloc_internals.arenas.mutex);
	bool released_mutex = false;	// Ugly but I don't care anymore
	if (!new_size_ok_for_arena(chunk_alloc_sz(chunk), req_size))
	{
		retval = copy_chunk_in_new_alloc(ptr, chunk, req_size, &released_mutex);
	}
	else if (size < chunk_alloc_sz(chunk))
	{
		try_shrink_chunk_for_requested_size(chunk, req_size);
		retval = (void*)chunk + ALIGN_MALLOC(sizeof(t_chunk));
	}
	else
	{
		retval = enlarge_or_alloc_new(ptr, chunk, req_size, &released_mutex);
	}
	if (released_mutex == false)
		pthread_mutex_unlock(&g_malloc_internals.arenas.mutex);
	return retval;
}
