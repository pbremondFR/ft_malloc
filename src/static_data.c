/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   static_data.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 16:39:14 by pbremond          #+#    #+#             */
/*   Updated: 2024/04/19 19:50:27 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_structs.h"

t_malloc_internals	g_malloc_internals = {
	/* On this specific architecture, this is technically unnecessary. PTHREAD_MUTEX_INITIALIZER
	 * is equivalent to zero-initialization, and static variables as well as partial-initialization
	 * always zero-initialize the object. But I guess it's better to be safe/portable.
	*/
	.arenas = { false, PTHREAD_MUTEX_INITIALIZER, NULL, NULL, NULL },
	.loaded_options = false,
	// This cast is necessary under clang but not GCC, go figure
	.options = (t_malloc_options){
		.tiny_alloc_max_sz = TINY_ALLOC_MAX_SZ,
		.small_alloc_max_sz = SMALL_ALLOC_MAX_SZ,
		.check_errors = 0x2
	},
};

_Thread_local t_malloc_arenas	*g_arenas = NULL;
