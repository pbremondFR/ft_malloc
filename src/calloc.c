/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   calloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 16:33:15 by pbremond          #+#    #+#             */
/*   Updated: 2024/02/28 17:53:35 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include <errno.h>

void	*CALLOC(size_t nb_elem, size_t size)
{
	(void)nb_elem, (void)size;
	errno = ENOMEM;
	return NULL;
}
