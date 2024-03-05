/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   calloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 16:33:15 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/05 21:42:00 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
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
		return MALLOC(nb_elem * size);
	}
}
