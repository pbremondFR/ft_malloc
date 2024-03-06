/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_options.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 21:53:41 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/06 16:37:24 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE
#include "ft_malloc_structs.h"
#include "libft.h"
#include <stdlib.h>
#include <stdatomic.h>

void	malloc_load_options()
{
	ft_putstr("Setting malloc options\n");

	// Default values are loaded from .data segment because it's a global variable

	g_malloc_internals.loaded_options = true;
}
