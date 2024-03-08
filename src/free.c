/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 16:32:51 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/07 23:08:08 by pbremond         ###   ########.fr       */
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

SHARED_LIB_EXPORT
void	FREE(void *ptr)
{
	// dbg_print(YEL"IN FREE"RESET"\n");
	if (ptr == NULL)
		return;
	t_chunk *chunk = ptr - ALIGN_MALLOC(sizeof(*chunk));

	// dbg_print(YEL"ptr:   %p"RESET"\n", ptr);
	// dbg_print(YEL"chunk: %p"RESET"\n", chunk);

	remove_large_chunk_from_list(chunk);
	size_t size = chunk->size & CHUNK_SIZE_MASK;
	if (munmap(chunk, size) != 0)
	{
		dbg_print("ERROR when freeing shit\n");
		dbg_print("Chunk raw size is %zu\n", chunk->size);
	}
	// dbg_print(YEL"OUT OF FREE"RESET"\n");
}
