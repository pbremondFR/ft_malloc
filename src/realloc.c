/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 17:50:53 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/05 21:37:28 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include "ft_malloc_structs.h"
#include "ft_malloc_defines.h"
#include "ft_malloc_utils.h"
#include "libft.h"
#include <errno.h>

SHARED_LIB_EXPORT
void	*REALLOC(void *ptr, size_t size)
{
	dbg_print("CALLED REALLOC\n");

	if (ptr == NULL)
		return MALLOC(size);
	else if (size == 0)
	{
		FREE(ptr);
		return NULL;
	}

	void	*newptr;
	t_chunk *chunk = ptr - sizeof(*chunk);
	size_t old_size = (chunk->size & CHUNK_SIZE_MASK);

	dbg_print("With size %zu & old size %zu\n", size, old_size);

	newptr = MALLOC(size);
	if (newptr == NULL)
	{
		FREE(ptr);
		return (NULL);
	}
	if (old_size - sizeof(t_chunk) < size)
		ft_memcpy(newptr, ptr, old_size - sizeof(t_chunk));
	else
		ft_memcpy(newptr, ptr, size);
	FREE(ptr);
	return (newptr);
}
