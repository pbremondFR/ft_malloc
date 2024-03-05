/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 13:18:42 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/05 21:29:50 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
# define MALLOC_H
# include <stddef.h>

/*
 * Use preproc defines to give a different name to these functions in debug.
 * This allows comparing behaviour of "real" malloc with my own. Don't use
 * 'ft' prefix because of the stupid ft_calloc in libft.
*/
# ifndef NDEBUG
#  define MALLOC	my_malloc
#  define CALLOC	my_calloc
#  define REALLOC	my_realloc
#  define FREE		my_free
# else
#  define MALLOC	malloc
#  define CALLOC	calloc
#  define REALLOC	realloc
#  define FREE		free
# endif

void	*MALLOC(size_t size);
// Implement calloc because it's required to correctly replace glibc's malloc:
// https://www.gnu.org/software/libc/manual/html_node/Replacing-malloc.html
void	*CALLOC(size_t nb_elem, size_t size);
void	*REALLOC(void *ptr, size_t size);
void	FREE(void *ptr);

void	show_alloc_mem();

#endif
