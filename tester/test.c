/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 23:56:44 by pbremond          #+#    #+#             */
/*   Updated: 2024/04/22 18:55:09 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include "ft_malloc_defines.h"
#include "ft_malloc_structs.h"
#include "libft.h"
#include "ansi_color.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>

__attribute__((unused))
static void	do_static_asserts()
{
	static_assert(ALIGN_MALLOC(0x123) % MALLOC_ALIGNMENT == 0, "Incorrect alignment macro");
	static_assert(ALIGN_MALLOC(0x2) % MALLOC_ALIGNMENT == 0, "Incorrect alignment macro");
	static_assert(ALIGN_MALLOC(0x0) % MALLOC_ALIGNMENT == 0, "Incorrect alignment macro");

	static_assert(ALIGN_TO(0x123, 4) % 4 == 0, "Incorrect alignment macro");
	static_assert(ALIGN_TO(0x2, 4) % 4 == 0, "Incorrect alignment macro");
	static_assert(ALIGN_TO(0x0, 4) % 4 == 0, "Incorrect alignment macro");

	static_assert(ALIGN_TO(0x123, 16) % 16 == 0, "Incorrect alignment macro");
	static_assert(ALIGN_TO(0x2, 16) % 16 == 0, "Incorrect alignment macro");
	static_assert(ALIGN_TO(0x0, 16) % 16 == 0, "Incorrect alignment macro");

	static_assert(MALLOC_ALIGNMENT == 16, "Possibly bad malloc alignment value");
}

static void	newtest(void)
{
	static int	i = 1;
	static char	buf[64] = {0};

	snprintf(buf, sizeof(buf), "\e[0;30;47m===============TEST %d===============\e[0m\n", i++);
	ft_putstr(buf);
}

static void	*routine(void *param)
{
	(void)param;
	ft_putstr(RED"Starting thread..."RESET"\n");
	void *mem_tiny = MALLOC(TINY_ALLOC_MAX_SZ / 2);
	void *mem_small = MALLOC(SMALL_ALLOC_MAX_SZ / 2);
	void *mem_large = MALLOC(SMALL_ALLOC_MAX_SZ * 2);
	sleep(2);

	FREE(mem_tiny);
	FREE(mem_small);
	FREE(mem_large);
	ft_putstr(BBLU"Exiting thread..."RESET"\n");
	return NULL;
}

int main()
{
	srand(time(NULL));

	newtest();
	{
		void *tiny1 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny2 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny3 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny4 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny5 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		SHOW_ALLOC_MEM();
		FREE(tiny3);
		SHOW_ALLOC_MEM();
		FREE(tiny4);
		SHOW_ALLOC_MEM();
		FREE(tiny2);
		SHOW_ALLOC_MEM();
		FREE(tiny1);
		SHOW_ALLOC_MEM();
		FREE(tiny5);
		SHOW_ALLOC_MEM();
	}
	newtest();
	{
		void *tiny1 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny2 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny3 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny4 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny5 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		SHOW_ALLOC_MEM();
		FREE(tiny3);
		SHOW_ALLOC_MEM();
		tiny3 = MALLOC(TINY_ALLOC_MAX_SZ / 4);
		SHOW_ALLOC_MEM();
		FREE(tiny1);
		FREE(tiny2);
		FREE(tiny3);
		FREE(tiny4);
		FREE(tiny5);
	}
	newtest();
	{
		void *tiny1 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *small1 = MALLOC(SMALL_ALLOC_MAX_SZ / 2);
		void *large1 = MALLOC(SMALL_ALLOC_MAX_SZ * 2);
		show_alloc_mem();
		void *tiny2 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *small2 = MALLOC(SMALL_ALLOC_MAX_SZ / 2);
		void *large2 = MALLOC(SMALL_ALLOC_MAX_SZ * 2);
		show_alloc_mem();
		FREE(tiny1);
		FREE(tiny2);
		FREE(small1);
		FREE(small2);
		FREE(large1);
		FREE(large2);
	}
	newtest();
	{
		SHOW_ALLOC_MEM();
		void *mem[SMALL_HEAP_MIN_ELEM * 8];
		for (size_t i = 0; i < SIZEOF_ARRAY(mem); ++i)
			mem[i] = MALLOC(SMALL_ALLOC_MAX_SZ);
		SHOW_ALLOC_MEM();
		for (size_t i = 0; i < SIZEOF_ARRAY(mem); ++i)
			FREE(mem[i]);
		SHOW_ALLOC_MEM();
	}
	newtest();
	{
		for (int i = 0; i < 10; ++i)
		{
			int alloc_size = rand() % 8192;
			void *mem = MALLOC(alloc_size);
			bool aligned = (long)mem % MALLOC_ALIGNMENT == 0;
			printf("mem: %p - size: %d - aligned: %s\n", mem, alloc_size, aligned ? "TRUE" : "FALSE");
			FREE(mem);
		}
	}
	newtest();
	{
		const char	str1[] = "Salut mon pote comment ca av fhdjsk fhjkdasl fhklsdjahfjkladshfjklashfjkl\n";
		const char	str2[] = "jkfdhkasjlf hjsdkafhdjksafhjkdsalfhjkdasl connard\n";

		char *truc = MALLOC(sizeof(str1));
		ft_strlcpy(truc, str1, sizeof(str1));

		char *muche = MALLOC(sizeof(str2));
		ft_strlcpy(muche, str2, sizeof(str2));

		ft_putstr(truc);
		ft_putstr(muche);
		show_alloc_mem();
		FREE(muche);
		FREE(truc);
	}
	newtest();
	{
		char *foo = MALLOC(64);
		char *bar = MALLOC(32);
		show_alloc_mem();
		FREE(bar);
		FREE(foo);
	}
	newtest();
	{
		char *foo = MALLOC(64);
		char *bar = MALLOC(32);
		show_alloc_mem();
		FREE(foo);
		FREE(bar);
	}
	newtest();
	{
		void *tiny = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		SHOW_ALLOC_MEM();
		tiny = REALLOC(tiny, TINY_ALLOC_MAX_SZ);
		SHOW_ALLOC_MEM();
		FREE(tiny);
	}
	newtest();
	{
		void *tiny1 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny2 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny3 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny4 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		SHOW_ALLOC_MEM();
		FREE(tiny3);
		tiny2 = REALLOC(tiny2, TINY_ALLOC_MAX_SZ);
		SHOW_ALLOC_MEM();
		FREE(tiny1);
		FREE(tiny2);
		FREE(tiny4);
	}
	newtest();
	{
		void *tiny1 = MALLOC(TINY_ALLOC_MAX_SZ / 4);
		void *tiny2 = MALLOC(TINY_ALLOC_MAX_SZ / 4);
		void *tiny3 = MALLOC(TINY_ALLOC_MAX_SZ / 4);
		void *tiny4 = MALLOC(TINY_ALLOC_MAX_SZ / 4);
		SHOW_ALLOC_MEM();
		FREE(tiny1);
		FREE(tiny3);
		SHOW_ALLOC_MEM();
		tiny2 = REALLOC(tiny2, TINY_ALLOC_MAX_SZ / 4 * 3);
		SHOW_ALLOC_MEM();
		FREE(tiny2);
		FREE(tiny4);
	}
	newtest();
	{
		void *tiny1 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny2 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny3 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		SHOW_ALLOC_MEM();
		tiny2 = REALLOC(tiny2, TINY_ALLOC_MAX_SZ / 4);
		SHOW_ALLOC_MEM();
		FREE(tiny1);
		FREE(tiny2);
		FREE(tiny3);
	}
	newtest();
	{
		void *tiny1 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny2 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		void *tiny3 = MALLOC(TINY_ALLOC_MAX_SZ / 2);
		SHOW_ALLOC_MEM();
		tiny2 = REALLOC(tiny2, TINY_ALLOC_MAX_SZ * 2);
		SHOW_ALLOC_MEM();
		FREE(tiny1);
		FREE(tiny2);
		FREE(tiny3);
	}
	newtest();
	{
		void *small1 = MALLOC(SMALL_ALLOC_MAX_SZ / 2);
		void *small2 = MALLOC(SMALL_ALLOC_MAX_SZ / 2);
		void *small3 = MALLOC(SMALL_ALLOC_MAX_SZ / 2);
		SHOW_ALLOC_MEM();
		small2 = REALLOC(small2, SMALL_ALLOC_MAX_SZ * 2);
		SHOW_ALLOC_MEM();
		FREE(small1);
		FREE(small2);
		FREE(small3);
		SHOW_ALLOC_MEM();
	}
	newtest();
	{
		void *small1 = MALLOC(SMALL_ALLOC_MAX_SZ / 2);
		void *small2 = MALLOC(SMALL_ALLOC_MAX_SZ / 2);
		void *small3 = MALLOC(SMALL_ALLOC_MAX_SZ / 2);
		SHOW_ALLOC_MEM();
		small2 = REALLOC(small2, TINY_ALLOC_MAX_SZ / 2);
		SHOW_ALLOC_MEM();
		FREE(small1);
		FREE(small2);
		FREE(small3);
	}
	newtest();
	{
		pthread_t	threads[8];

		for (size_t i = 0; i < SIZEOF_ARRAY(threads); ++i)
			pthread_create(&threads[i], NULL, routine, NULL);
		// All threads allocate and wait for us to print memory before freeing
		sleep(1);
		SHOW_ALLOC_MEM();
		for (size_t i = 0; i < SIZEOF_ARRAY(threads); ++i)
		{
			pthread_join(threads[i], NULL);
		}
		SHOW_ALLOC_MEM();
	}
	newtest();
	{
		pthread_t	threads[64];

		for (size_t i = 0; i < SIZEOF_ARRAY(threads); ++i)
			pthread_create(&threads[i], NULL, routine, NULL);
		// All threads allocate and wait for us to print memory before freeing
		sleep(1);
		SHOW_ALLOC_MEM();
		for (size_t i = 0; i < SIZEOF_ARRAY(threads); ++i)
			pthread_join(threads[i], NULL);
	}
	SHOW_ALLOC_MEM();
	newtest();
	{
		// Trigger double free error
		void *mem = MALLOC(SMALL_ALLOC_MAX_SZ / 2);
		FREE(mem);
		FREE(mem);
	}
	return 0;
}
