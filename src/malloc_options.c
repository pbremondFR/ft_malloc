/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_options.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 21:53:41 by pbremond          #+#    #+#             */
/*   Updated: 2024/04/19 18:23:30 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE
#include "ft_malloc_structs.h"
#include "libft.h"
#include <stdlib.h>
#include <stdatomic.h>

void	malloc_load_options()
{
	// Default values are loaded from .data segment because it's a global variable

	g_malloc_internals.loaded_options = true;
}
