/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 16:32:51 by pbremond          #+#    #+#             */
/*   Updated: 2024/04/17 17:03:41 by pbremond         ###   ########.fr       */
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
	// pthread_mutex_lock(&g_malloc_internals.arenas.mutex);

	t_chunk	*prev = g_malloc_internals.arenas.big_allocs;
	while (prev != chunk && prev->next != NULL && prev->next != chunk)
		prev = prev->next;
	if (prev == chunk)
		g_malloc_internals.arenas.big_allocs = chunk->next;
	else
		prev->next = chunk->next;

	// pthread_mutex_unlock(&g_malloc_internals.arenas.mutex);
}

static void	free_chunk(t_chunk *chunk)
{
	bool collapsed_prev = false;
	bool collapsed_next = false;
	tarace("chunk: %p, chunk->next: %p, chunk size %zu (alloced %zu)\n",
		chunk, chunk->next, chunk_sz(chunk), chunk_alloc_sz(chunk));

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
		// dbg_print(YEL"Collapsing next chunk"RESET"\n");
		collapsed_next = true;
		size_t flags = chunk->size & ~CHUNK_SIZE_MASK;
		chunk->size = (chunk_sz(chunk) + chunk_sz(chunk->next)) | flags;
		chunk->next = chunk->next->next;
	}

	// Collapse previous chunk if it's free
	if (chunk->size & FLAG_CHUNK_PREV_FREE)
	{
		// dbg_print(YEL"Collapsing previous chunk"RESET"\n");
		const size_t *p_prev_size = (void*)chunk - sizeof(size_t);
		const size_t prev_size = *p_prev_size;
		tarace("prev size ptr is %p with value %zu (%#x)\n", p_prev_size, prev_size, prev_size);
		t_chunk	*prev = (void*)chunk - prev_size;
		// size_t flags = prev->size & ~CHUNK_SIZE_MASK;
		prev->size = (chunk_sz(chunk) + chunk_sz(prev));
		prev->next = chunk->next;
		chunk = prev;
		collapsed_prev = true;
	}

	chunk->size |= FLAG_CHUNK_FREE;
	if (chunk->next)	// Set tag to indicate that previous chunk is free
		chunk->next->size |= FLAG_CHUNK_PREV_FREE;

	size_t *trailing_size_tag = (void*)chunk + chunk_sz(chunk) - sizeof(size_t);
	tarace("Collapsed %d/%d, chunk: %p, chunk size: %zu(%#lx), trailing size tag: %p\n",
		collapsed_prev, collapsed_next, chunk, chunk_sz(chunk), chunk_sz(chunk), trailing_size_tag);
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
	// ft_putstr(YEL"IN FREE"RESET"\n");
	const uintptr_t	pagesize = getpagesize();
	char buf[256];
	snprintf(buf, sizeof(buf), "Pagesize: %zu, as ptr: %p\n", pagesize, (void*)pagesize);
	// ft_putstr(buf);

	t_chunk *chunk = ptr - ALIGN_MALLOC(sizeof(*chunk));
	snprintf(buf, sizeof(buf), "Freeing %p (chunk %p)\n", ptr, chunk);
	ft_putstr(buf);
	if ((size_t)ptr < pagesize)
	// if (ptr == NULL)
	{
		if (ptr != NULL) {
			ft_putstr(REDB"CORRUPTED MEMORY OH NO"RESET"\n");
			ft_putstr_fd(REDB"CORRUPTED MEMORY OH NO"RESET"\n", 2);
		}
		return;
	}
	pthread_mutex_lock(&g_malloc_internals.arenas.mutex);

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
	// dbg_print(YEL"OUT OF FREE"RESET"\n");
}
