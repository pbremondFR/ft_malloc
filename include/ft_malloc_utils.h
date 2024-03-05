/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc_utils.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 15:18:25 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/05 20:24:31 by pbremond         ###   ########.fr       */
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

#endif
