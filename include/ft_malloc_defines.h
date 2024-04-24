/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc_defines.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 21:34:51 by pbremond          #+#    #+#             */
/*   Updated: 2024/04/24 15:24:02 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MALLOC_DEFINES_H
# define FT_MALLOC_DEFINES_H

# include <stddef.h>
# include <stdalign.h>

# define SIZEOF_ARRAY(x)	(sizeof(x) / sizeof(x[0]))

// Completely unnecessary but I don't care, wanted to try it out once
# define likely(x)		__builtin_expect(!!(x), 1)
# define unlikely(x)	__builtin_expect(!!(x), 0)

# define SHARED_LIB_EXPORT	__attribute__((__visibility__("default")))

# define ALIGN_TO(x, size)	(((size_t)(x) + (size-1)) & ~(size-1))
# define MALLOC_ALIGNMENT	(alignof(max_align_t))
# define ALIGN_MALLOC(x)	ALIGN_TO(x, MALLOC_ALIGNMENT)

# define TINY_ALLOC_MAX_SZ		128
# define TINY_HEAP_MIN_ELEM		128
// See M_MMAP_THRESHOLD, is by default 128 * 1024
# define SMALL_ALLOC_MAX_SZ		(128 * 1024)
# define SMALL_HEAP_MIN_ELEM	128

# define NUM_ARENAS			16

# ifndef NDEBUG
#  define dbg_print(...)		dprintf(2, __VA_ARGS__)
# else
#  define dbg_print(...)
# endif

# ifndef FT_BONUS
#  define SHOW_ALLOC_MEM show_alloc_mem
# else
#  define SHOW_ALLOC_MEM show_alloc_mem_ex
#endif

#endif
