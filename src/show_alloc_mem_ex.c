/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem_ex.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/09 19:31:33 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/09 21:04:01 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_structs.h"
#include "ft_malloc_utils.h"
#include "libft.h"
#include "ansi_color.h"

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
			bool is_free = chunk->size & FLAG_CHUNK_FREE;
			const void *mem = (char*)chunk + ALIGN_MALLOC(sizeof(t_chunk));
			size_t size = chunk_alloc_sz(chunk);
			if (is_free)
				snprintf(buf, sizeof(buf), YEL"%p - %p : %zu bytes"RESET"\n", mem, mem + size, size);
			else
			{
				snprintf(buf, sizeof(buf), GRN"%p - %p : %zu bytes"RESET"\n", mem, mem + size, size);
				sum += size;
			}
			ft_putstr(buf);
		}
		heaps_list = heaps_list->next;
	}
	return sum;
}

static size_t	print_large_allocs(const t_chunk *chunks)
{
	char	buf[256] = {0};
	size_t	sum = 0;

	ft_putstr("LARGE:\n");
	for (; chunks != NULL; chunks = chunks->next)
	{
		const void *mem = (char*)chunks + sizeof(t_chunk);
		size_t size = (chunks->size & CHUNK_SIZE_MASK) - sizeof(t_chunk);
		sum += size;
		snprintf(buf, sizeof(buf), "%p - %p : %zu bytes\n", mem, mem + size, size);
		ft_putstr(buf);
	}
	return sum;
}

SHARED_LIB_EXPORT
void	show_alloc_mem_ex()
{
	char	buf[256] = {0};
	size_t	sum = 0;

	for (unsigned int i = 0; i < SIZEOF_ARRAY(g_malloc_internals.arenas); ++i)
	{
		t_malloc_arenas *arenas = &g_malloc_internals.arenas[i];
		if (arenas->num_threads == 0)
			continue;
		snprintf(buf, sizeof(buf), "\nArena %d:\n", i);
		ft_putstr(buf);

		pthread_mutex_lock(&arenas->mutex);
		ft_putstr("TINY:\n");
		sum += print_allocs_from_heaps(arenas->tiny_heaps);
		ft_putstr("SMALL:\n");
		sum += print_allocs_from_heaps(arenas->small_heaps);
		sum += print_large_allocs(arenas->big_allocs);
		pthread_mutex_unlock(&arenas->mutex);
	}
	snprintf(buf, sizeof(buf), "Total: %zu\n", sum);
	ft_putstr(buf);
}
