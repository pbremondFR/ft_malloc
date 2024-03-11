/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc_utils.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 15:18:25 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/10 20:20:22 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MALLOC_UTILS_H
# define FT_MALLOC_UTILS_H

# define Min(a,b)					\
	({								\
		__typeof__ (a) _a = (a);	\
		__typeof__ (b) _b = (b);	\
		_a < _b ? _a : _b;			\
	})

# define Max(a,b)					\
	({								\
		__typeof__ (a) _a = (a);	\
		__typeof__ (b) _b = (b);	\
		_a > _b ? _a : _b;			\
	})

inline size_t	chunk_sz(const t_chunk *c)			{ return c->size & CHUNK_SIZE_MASK; }
inline size_t	chunk_alloc_sz(const t_chunk *c)	{ return chunk_sz(c) - sizeof(*c); }

void	rec_putnbr(int n);
void	rec_putsize_t(size_t n);
void	rec_putnbr_base(long n, const char *base);

void	malloc_load_options();
t_heap	*create_new_heap(size_t heap_size);
void	insert_heap_in_list(t_heap **list, t_heap *new_heap);

t_chunk	*find_best_chunk_for_alloc(t_heap const *heaps, size_t req_size);
t_chunk	*alloc_chunk_from_heaps(t_heap **heap_lst, size_t req_size, size_t new_heap_min_sz);

#endif
