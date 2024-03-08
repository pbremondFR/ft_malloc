/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc_structs.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 00:27:48 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/08 02:17:33 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MALLOC_STRUCTS_H
# define FT_MALLOC_STRUCTS_H
# include <stddef.h>
# include <stdbool.h>
# include <stdalign.h>
# include <stdatomic.h>
# include <pthread.h>
# include "ft_malloc_defines.h"

# define CHUNK_SIZE_MASK	(~(MALLOC_ALIGNMENT - 1))
# define FLAG_CHUNK_FREE		0x1
# define FLAG_CHUNK_PREV_FREE	0x2
# define FLAG_CHUNK_MMAPPED		0x4

# define MAX_ARENAS	8

typedef pthread_mutex_t	t_mutex;

typedef struct s_chunk
{
	// size_t			prev_size;
	size_t			size;	// Also includes flags like GNU malloc
	struct s_chunk	*next;	// Do NOT read when block isn't free, except for mmap'd blocks
}	t_chunk;

typedef struct s_heap
{
	size_t			size;		// Size of mmap'd memory
	struct s_heap	*next;
	struct s_chunk	*chunks;
}	t_heap;

typedef struct s_arena
{
	struct s_heap	*heaps;	// Linked list of heaps
}	t_arena;

typedef struct
{
	atomic_size_t	num_threads;
	t_mutex			mutex;
	t_heap			*tiny_heaps;
	t_heap			*small_heaps;
	t_chunk			*big_allocs;
}	t_malloc_arenas;

typedef struct
{
	int	tiny_alloc_max_sz;
	int	small_alloc_max_sz;
	int	optimistic_arena_assign;
	int	check_errors;
}	t_malloc_options;

typedef _Atomic t_malloc_options atomic_options;

typedef struct
{
	t_malloc_arenas	arenas[NUM_ARENAS];	// Array of arenas
	// Put other coolstuff/options for malloc in there, for debugging or
	// bonuses like dynamically adjustable options
	atomic_bool		loaded_options;
	atomic_options	options;

}	t_malloc_internals;

extern t_malloc_internals	g_malloc_internals;

_Thread_local
extern t_malloc_arenas		*g_arenas;

#endif
