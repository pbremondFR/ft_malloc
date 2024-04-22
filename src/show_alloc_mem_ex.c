/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem_ex.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/09 19:31:33 by pbremond          #+#    #+#             */
/*   Updated: 2024/04/22 18:55:24 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_structs.h"
#include "ft_malloc_utils.h"
#include "libft.h"
#include "ansi_color.h"

static void	fuck_you_for_not_allowing_printf_im_not_recoding_it_again(const char * restrict colour,
	const void *mem, size_t size)
{
	ft_putstr(colour);
	ft_putstr("0x"); rec_putnbr_base((long)mem, BASE_HEXA_LOWER); ft_putstr(" - 0x");
	rec_putnbr_base((long)mem + size, BASE_HEXA_LOWER); ft_putstr(" : ");
	rec_putsize_t(size); ft_putstr(" bytes\n");
	ft_putstr(RESET);
}

static size_t	print_allocs_from_heaps(t_heap const *heaps_list)
{
	size_t		sum = 0;
	const char	*percent_color[] = {RED, RED, YEL, YEL, YEL, GRN, GRN, GRN, GRN, GRN, GRN};

	while (heaps_list)
	{
		size_t avail = 0;

		for (t_chunk const *chunk = heaps_list->chunks;
			chunk && (char*)chunk < (char*)heaps_list + heaps_list->size;
			chunk = chunk->next)
		{
			bool is_free = chunk->size & FLAG_CHUNK_FREE;
			const void *mem = (char*)chunk + ALIGN_MALLOC(sizeof(t_chunk));
			size_t size = chunk_alloc_sz(chunk);
			if (is_free)
			{
				fuck_you_for_not_allowing_printf_im_not_recoding_it_again(YEL, mem, size);
				avail += size;
			}
			else
			{
				fuck_you_for_not_allowing_printf_im_not_recoding_it_again(GRN, mem, size);
				sum += size;
			}
		}
		size_t heap_total_sz = heaps_list->size - ALIGN_MALLOC(sizeof(t_heap)) - sizeof(t_chunk);
		size_t percent_avail = avail / (heap_total_sz / 100);
		ft_putstr(percent_color[percent_avail / 10]);
		rec_putsize_t(percent_avail);
		ft_putstr("%"RESET" of heap available\n");

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
		fuck_you_for_not_allowing_printf_im_not_recoding_it_again(RESET, mem, size);
	}
	return sum;
}

SHARED_LIB_EXPORT
void	show_alloc_mem_ex()
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
