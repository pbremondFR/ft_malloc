/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 16:32:51 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/09 02:32:04 by pbremond         ###   ########.fr       */
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
	pthread_mutex_lock(&g_arenas->mutex);

	t_chunk	*prev = g_arenas->big_allocs;
	while (prev != chunk && prev->next != NULL && prev->next != chunk)
		prev = prev->next;
	if (prev == chunk)
		g_arenas->big_allocs = chunk->next;
	else
		prev->next = chunk->next;

	pthread_mutex_unlock(&g_arenas->mutex);
}

static void	free_chunk(t_chunk *chunk)
{
	// Collapse next chunk with currently free'd chunk
	// ft_putstr(BYEL"Freeing chunk"RESET"\n");
	if (chunk->next && chunk->next->size & FLAG_CHUNK_FREE)
	{
		// ft_putstr(YEL"Collapsing next chunk"RESET"\n");
		size_t flags = chunk->size & ~CHUNK_SIZE_MASK;
		chunk->size = (chunk_sz(chunk) + chunk_sz(chunk->next)) | flags;
		chunk->next = chunk->next->next;
	}
	if (chunk->next)	// Set tag to indicate that previous chunk is free
		chunk->next->size |= FLAG_CHUNK_PREV_FREE;
	else
		// ft_putstr("No next chunk\n");
	if (chunk->size & FLAG_CHUNK_PREV_FREE)
	{
		// ft_putstr(YEL"Collapsing previous chunk"RESET"\n");
		size_t	*prev_size = (void*)chunk - sizeof(size_t);
		t_chunk	*prev = (void*)chunk - *prev_size;
		size_t flags = prev->size & ~CHUNK_SIZE_MASK;
		prev->size = (chunk_sz(chunk) + chunk_sz(prev)) | flags;
		prev->next = chunk->next;
		chunk = prev;
	}
	chunk->size |= FLAG_CHUNK_FREE;
	size_t *trailing_size_tag = (void*)chunk + chunk_sz(chunk) - sizeof(size_t);
	*trailing_size_tag = chunk_sz(chunk);
}

SHARED_LIB_EXPORT
void	FREE(void *ptr)
{
	// dbg_print(YEL"IN FREE"RESET"\n");
	if (ptr == NULL)
		return;
	t_chunk *chunk = ptr - ALIGN_MALLOC(sizeof(*chunk));

	if (chunk->size & FLAG_CHUNK_MMAPPED)
	{
		remove_large_chunk_from_list(chunk);
		if (munmap(chunk, chunk_sz(chunk)) != 0) {
			dbg_print("OH FUCK\n");
		}
	}
	else
	{
		free_chunk(chunk);
		return;
	}
	// dbg_print(YEL"OUT OF FREE"RESET"\n");
}
