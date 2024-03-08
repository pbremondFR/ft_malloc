/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc_utils.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 15:18:25 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/08 02:39:59 by pbremond         ###   ########.fr       */
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

void	rec_putnbr(int n);
void	rec_putnbr_base(int n, const char *base);

void	malloc_load_options();
t_heap	*create_new_heap(size_t heap_size);
void	insert_heap_in_list(t_heap **list, t_heap *new_heap);

#endif
