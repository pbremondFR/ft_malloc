/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 16:32:51 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/06 22:17:51 by pbremond         ###   ########.fr       */
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

static void	remove_large_chunk_from_list(t_chunk *chunk)
{
	t_chunk	*next = chunk->next;
	pthread_mutex_lock(&g_arenas->mutex);
	t_chunk	*prev = g_arenas->big_allocs;
	if (g_arenas->big_allocs == NULL)
		return;
	for (; prev->next != NULL; prev = prev->next)
	{
		if (prev->next == chunk)
			break;
	}
	if (prev == chunk)
		g_arenas->big_allocs = NULL;
	else
		prev->next = next;
	pthread_mutex_unlock(&g_arenas->mutex);
}

SHARED_LIB_EXPORT
void	FREE(void *ptr)
{
	ft_putstr("IN FREE\n");
	if (ptr == NULL)
		return;
	t_chunk *chunk = ptr - sizeof(*chunk);

	dbg_print("ptr:   %p\n", ptr);
	dbg_print("chunk: %p\n", chunk);

	remove_large_chunk_from_list(chunk);
	size_t size = chunk->size;
	if (munmap(chunk, size & 0b111) != 0)
	{
		dbg_print("ERROR when freeing shit\n");
		dbg_print("Size is %zu\n", chunk->size);
	}
}
