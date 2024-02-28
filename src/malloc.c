/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 13:16:31 by pbremond          #+#    #+#             */
/*   Updated: 2024/02/28 20:50:54 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include "libft.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

void	*MALLOC(size_t size)
{
	(void)size;
	write(STDOUT_FILENO, __func__, sizeof(__func__));
	ft_putchar('\n');
	errno = ENOMEM;
	return NULL;
}
