/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 16:32:51 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/01 01:18:55 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include <sys/mman.h>

#include <stdio.h>

void	FREE(void *ptr)
{
	size_t *size = ptr - sizeof(size_t);
	dbg_print("Size: %zu\n", *size);
	munmap(size, *size);
}
