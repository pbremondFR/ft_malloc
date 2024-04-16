/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   calloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 16:33:15 by pbremond          #+#    #+#             */
/*   Updated: 2024/04/15 14:38:03 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include "ft_malloc_structs.h"
#include "ft_malloc_defines.h"
#include <errno.h>
#include <stdint.h>
#include "libft.h"

SHARED_LIB_EXPORT
void	*CALLOC(size_t nb_elem, size_t size)
{
	dbg_print("CALLED CALLOC\n");

	if (SIZE_MAX / size < nb_elem)	// size_t overflow
	{
		errno = ENOMEM;
		return NULL;
	}
	else
	{
		const t_malloc_options opt = g_malloc_internals.options;

		void *mem = MALLOC(nb_elem * size);
		if (!mem)	// errno already set in malloc
			return NULL;
		if (size > (size_t)opt.small_alloc_max_sz)
			ft_bzero(mem, nb_elem * size);	// TODO: optimize with 64-bit wide call
		return mem;
	}
}
