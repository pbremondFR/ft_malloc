/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_structs.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 00:27:48 by pbremond          #+#    #+#             */
/*   Updated: 2024/02/29 15:16:25 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_STRUCTS_H
# define MALLOC_STRUCTS_H
# include <stddef.h>
# include <stdbool.h>
# include <stdalign.h>

# define CHUNK_SIZE_MASK	(alignof(void*) - 1)

struct s_chunk
{
	size_t			size;
	struct s_chunk	*next;
};

inline get_chunk_size(const struct s_chunk *chunk)
{
	return chunk->size & ~CHUNK_SIZE_MASK;
}

inline is_chunk_free(const struct s_chunk *chunk)
{
	return chunk->size & 0x1;
}

#endif
