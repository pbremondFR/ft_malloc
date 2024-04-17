/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 17:50:53 by pbremond          #+#    #+#             */
/*   Updated: 2024/04/17 16:37:35 by pbremond         ###   ########.fr       */
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

	errno = ENOMEM;
	return NULL;

	if (ptr == NULL)
		return MALLOC(size);
	else if (size == 0)
	{
		FREE(ptr);
		return NULL;
	}

	size_t	aligned_chunk_sz	= ALIGN_MALLOC(sizeof(t_chunk));
	t_chunk *chunk				= ptr - aligned_chunk_sz;
	size_t	old_size			= chunk_sz(chunk);
	size_t	old_alloc_size		= chunk_alloc_sz(chunk);

	dbg_print("With size %zu & old size %zu\n", size, old_size);

	void *newptr = MALLOC(size);
	if (newptr == NULL)
	{
		FREE(ptr);
		return (NULL);
	}
	if (old_alloc_size < size)
		ft_memcpy(newptr, ptr, old_alloc_size);
	else
		ft_memcpy(newptr, ptr, size);
	FREE(ptr);
	return (newptr);
}
