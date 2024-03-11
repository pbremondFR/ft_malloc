/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 19:23:02 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/10 21:13:49 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_structs.h"
#include "ft_malloc_utils.h"
#include "libft.h"

static size_t	print_allocs_from_heaps(t_heap const *heaps_list)
{
	char	buf[128] = {0};
	size_t	sum = 0;

	while (heaps_list)
	{
		for (t_chunk const *chunk = heaps_list->chunks;
			chunk && (char*)chunk < (char*)heaps_list + heaps_list->size;
			chunk = chunk->next)
		{
			if (chunk->size & FLAG_CHUNK_FREE)
				continue;
			const void *mem = (char*)chunk + ALIGN_MALLOC(sizeof(t_chunk));
			size_t size = chunk_alloc_sz(chunk);
			sum += size;
			snprintf(buf, sizeof(buf), "%p - %p : %zu bytes\n", mem, mem + size, size);
			ft_putstr(buf);
		}
		heaps_list = heaps_list->next;
	}
	return sum;
}

static size_t	print_large_allocs(const t_chunk *chunks)
{
	size_t	sum = 0;

	ft_putstr("LARGE:\n");
	for (; chunks != NULL; chunks = chunks->next)
	{
		const void *mem = (char*)chunks + sizeof(t_chunk);
		size_t size = chunk_alloc_sz(chunks);
		sum += size;
		// IDGAF, I an NOT recoding snprintf. This project isn't worth it.
		ft_putstr("0x"); rec_putnbr_base((long)mem, BASE_HEXA_LOWER); ft_putstr(" - 0x");
		rec_putnbr_base((long)mem + size, BASE_HEXA_LOWER); ft_putstr(" : ");
		rec_putsize_t(size); ft_putstr(" bytes\n");
	}
	return sum;
}

SHARED_LIB_EXPORT
void	show_alloc_mem()
{
	size_t	sum = 0;

	t_malloc_arenas *arenas = &g_malloc_internals.arenas;

	pthread_mutex_lock(&arenas->mutex);
	ft_putstr("TINY:\n");
	sum += print_allocs_from_heaps(arenas->tiny_heaps);
	ft_putstr("SMALL:\n");
	sum += print_allocs_from_heaps(arenas->small_heaps);
	sum += print_large_allocs(arenas->big_allocs);
	pthread_mutex_unlock(&arenas->mutex);

	ft_putstr("Total: "); rec_putsize_t(sum); ft_putstr(" bytes\n");
}
