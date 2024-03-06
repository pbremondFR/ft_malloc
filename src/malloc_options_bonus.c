/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_options_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 21:53:41 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/06 13:43:40 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE
#include "ft_malloc_structs.h"
#include "libft.h"
#include <stdlib.h>

static void override_malloc_options_from_env(t_malloc_options *options)
{
	typedef struct { const char *key; int *value; } kv_pair;

	kv_pair	parameters[] = {
		{"OPTIMISTIC_ARENA_ASSIGN",	&options->optimistic_arena_assign},
		{"TINY_ALLOC_MAX",			&options->tiny_alloc_max},
		{"SMALL_ALLOC_MAX",			&options->small_alloc_max},
		{"CHECK_ERRORS",			&options->check_errors},
	};

	for (unsigned int i = 0; i < SIZEOF_ARRAY(parameters); ++i)
	{
		const char *value_str = secure_getenv(parameters[i].key);
		if (value_str)
		{
			int value = ft_atoi(value_str);
			if (value != -1 || (value == -1 && *value_str == '-' && *(value_str + 1) == '1'))
				*parameters[i].value = value;
		}
	}
	options->loaded = true;
}

void	malloc_load_options()
{
	ft_putstr("Setting malloc options\n");
	t_malloc_options old_options, new_options;

	do
	{
		(void)sizeof(t_malloc_options);
		old_options = g_malloc_internals.options;
		new_options = old_options;

		// Override default values with environment variables
		override_malloc_options_from_env(&new_options);
	} while (!atomic_compare_exchange_strong(&g_malloc_internals.options, &old_options, new_options));


	dbg_print("# Optimistic arena assign: %d\n", new_options.optimistic_arena_assign);
	dbg_print("# Tiny alloc max: %d\n", new_options.tiny_alloc_max);
	dbg_print("# Small alloc max: %d\n", new_options.small_alloc_max);
	dbg_print("# Check errors: %d\n", new_options.check_errors);

	g_malloc_internals.loaded_options = true;
}
