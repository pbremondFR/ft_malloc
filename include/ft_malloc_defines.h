/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc_defines.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 21:34:51 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/05 21:41:40 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MALLOC_DEFINES_H
# define FT_MALLOC_DEFINES_H

# include <stddef.h>
# include <stdalign.h>

# define SIZEOF_ARRAY(x)	(sizeof(x) / sizeof(x[0]))

# define likely(x)		__builtin_expect(!!(x), 1)
# define unlikely(x)	__builtin_expect(!!(x), 0)

# define SHARED_LIB_EXPORT	__attribute__((__visibility__("default")))

# define ALIGN_TO(x, size)	(((size_t)(x) + (size-1)) & ~(size-1))
# define MALLOC_ALIGNMENT	(alignof(max_align_t))
# define ALIGN_MALLOC(x)	ALIGN_TO(x, MALLOC_ALIGNMENT)

# define PAGE_SIZE			getpagesize()
# define TINY_ALLOC_MAX		128
// See M_MMAP_THRESHOLD
// # define SMALL_ALLOC_MAX	128 * 1024
# define SMALL_ALLOC_MAX	512
# define TINY_ZONE_SIZE		ALIGN_TO(TINY_ALLOC_MAX * 100, PAGE_SIZE)
# define SMALL_ZONE_SIZE	ALIGN_TO(SMALL_ALLOC_MAX * 100, PAGE_SIZE)

# define NUM_ARENAS			16

# ifndef NDEBUG
#  define dbg_print(...)		dprintf(2, __VA_ARGS__)
# else
#  define dbg_print(...)
# endif

#endif
