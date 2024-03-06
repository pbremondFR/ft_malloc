/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_options_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 21:53:41 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/06 18:47:49 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE
#include "ft_malloc_structs.h"
#include "libft.h"
#include <stdlib.h>

static bool	valid_options(t_malloc_options const* options)
{
	return (options->small_alloc_max_sz > options->tiny_alloc_max_sz)
		&& (options->tiny_alloc_max_sz > 0);
}

static void override_malloc_options_from_env(t_malloc_options *options)
{
	typedef struct { const char *key; int *value; } kv_pair;
	t_malloc_options new_options = *options;

	kv_pair	parameters[] = {
		{"OPTIMISTIC_ARENA_ASSIGN",	&new_options.optimistic_arena_assign},
		{"TINY_ALLOC_MAX_SZ",		&new_options.tiny_alloc_max_sz},
		{"SMALL_ALLOC_MAX_SZ",		&new_options.small_alloc_max_sz},
		{"CHECK_ERRORS",			&new_options.check_errors},
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
	if (valid_options(&new_options))
		*options = new_options;
}

/*
 * I KNOW that the atomic struct is larger than the lock-free max size, but I
 * benchmarked _Atomic struct vs mutex and found either no significant changes
 * or a slight advantage when using _Atomic in this case (clang probably uses
 * a mutex under the hood). When combined with its sematic advantages
 * (no need to use mutex + is guaranteed to be correct), I prefer to use _Atomic.
 */
void	malloc_load_options()
{
	ft_putstr("Setting malloc options\n");
	t_malloc_options old_options, new_options;

	do
	{
		(void)sizeof(t_malloc_options);
		old_options = g_malloc_internals.options;
		new_options = old_options;

		// Another thread already did that in the meantime, no need to block
		if (g_malloc_internals.loaded_options == true)
			return;
		override_malloc_options_from_env(&new_options);
	} while (!atomic_compare_exchange_strong(&g_malloc_internals.options, &old_options, new_options));
	g_malloc_internals.loaded_options = true;

	dbg_print("# Optimistic arena assign: %d\n", new_options.optimistic_arena_assign);
	dbg_print("# Tiny alloc max: %d\n", new_options.tiny_alloc_max_sz);
	dbg_print("# Small alloc max: %d\n", new_options.small_alloc_max_sz);
	dbg_print("# Check errors: %d\n", new_options.check_errors);
}
