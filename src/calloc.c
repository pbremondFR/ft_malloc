/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   calloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 16:33:15 by pbremond          #+#    #+#             */
/*   Updated: 2024/04/18 14:51:15 by pbremond         ###   ########.fr       */
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

	if (nb_elem == 0 || size == 0)
		return NULL;
	if (SIZE_MAX / size < nb_elem)	// size_t overflow
	{
		errno = ENOMEM;
		return NULL;
	}
	else
	{
		const t_malloc_options opt = g_malloc_internals.options;
		const size_t req_alloc_sz = nb_elem * size;
		void *mem = MALLOC(req_alloc_sz);
		if (!mem)	// errno already set in malloc
			return NULL;
		if (req_alloc_sz <= (size_t)opt.small_alloc_max_sz)
			ft_bzero(mem, ALIGN_MALLOC(req_alloc_sz));	// TODO: optimize with 64-bit wide call
		return mem;
	}
}
