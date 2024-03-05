/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 16:32:51 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/05 21:35:46 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include "ft_malloc_structs.h"
#include "ft_malloc_defines.h"
#include "ft_malloc_utils.h"
#include <sys/mman.h>

#include <stdio.h>
#include <unistd.h>

SHARED_LIB_EXPORT
void	FREE(void *ptr)
{
	dbg_print("IN FREE\n");
	if (ptr == NULL)
		return;
	t_chunk *chunk = ptr - sizeof(*chunk);

	dbg_print("ptr:   %p\n", ptr);
	dbg_print("chunk: %p\n", chunk);

	size_t size = chunk->size;
	if (munmap(chunk, size & 0b111) != 0)
	{
		dbg_print("ERROR when freeing shit\n");
		dbg_print("Size is %zu\n", chunk->size);
	}
}
