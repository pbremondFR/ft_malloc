/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 23:56:44 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/07 13:58:11 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include "ft_malloc_defines.h"
#include "ft_malloc_structs.h"
#include "libft.h"
#include <stdio.h>
#include <unistd.h>
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

static void	print_settings()
{
	dbg_print("Page size: %d, %ld\n", getpagesize(), sysconf(_SC_PAGESIZE));
	dbg_print("Malloc alignment: %zu\n", MALLOC_ALIGNMENT);
}

int main()
{
	srand(time(NULL));

	print_settings();

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

	return 0;
}
