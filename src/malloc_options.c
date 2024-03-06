/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_options.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 21:53:41 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/05 22:36:41 by pbremond         ###   ########.fr       */
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

	// Assign default values to all options
	g_malloc_internals.options.optimistic_arena_assign = 0;
	g_malloc_internals.options.tiny_alloc_max = TINY_ALLOC_MAX;
	g_malloc_internals.options.small_alloc_max = SMALL_ALLOC_MAX;
	g_malloc_internals.options.check_errors = 1;

	g_malloc_internals.loaded_options = true;
}
