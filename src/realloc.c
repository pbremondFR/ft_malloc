/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 17:50:53 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/01 01:18:55 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include <errno.h>

void	*REALLOC(void *ptr, size_t size)
{
	(void)ptr, (void)size;
	errno = ENOMEM;
	return NULL;
}
