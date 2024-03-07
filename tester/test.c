/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 23:56:44 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/06 22:15:59 by pbremond         ###   ########.fr       */
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

static void	print_settings()
{
	dbg_print("Page size: %d, %ld\n", getpagesize(), sysconf(_SC_PAGESIZE));
	dbg_print("Malloc alignment: %zu\n", MALLOC_ALIGNMENT);
}

// static void	test_alignment()
// {
// 	void *test = MALLOC(11);
// 	if ((size_t)test % MALLOC_ALIGNMENT != 0)
// 	{
// 		ft_putstr("BAAAD ALIGN!!!!\n");
// 	}
// 	FREE(test);

// 	for (int i = 0; i < 10; ++i)
// 	{
// 		int size = rand() % 1024 * 1024;
// 		void *test2 = malloc(size);
// 		printf("### Size: %10d - Test pointer: %p ###\n", size, test2);
// 		free(test2);
// 	}
// }

int main()
{
	srand(time(NULL));

	print_settings();
	dbg_print("Thread id: %lu\n", pthread_self());

	const char msg[] = "Hello world!\n";
	ft_putstr("Message: ");
	ft_putstr(msg);

	char *test = MALLOC(sizeof(msg));
		show_alloc_mem();
	strcpy(test, msg);
	ft_putstr("Copy: ");
	ft_putstr(test);
	FREE(test);
		show_alloc_mem();

	// test_alignment();

	{
		ft_putstr("before truc\n");
		char *truc = ft_strdup("Salut mon pote comment ca av fhdjsk fhjkdasl fhklsdjahfjkladshfjklashfjkl\n");
		ft_putstr("after truc\n");
		char *muche = ft_strdup("jkfdhkasjlf hjsdkafhdjksafhjkdsalfhjkdasl connard\n");
		ft_putstr("after MUCHE\n");
		show_alloc_mem();
		ft_putstr(truc);
		ft_putstr(muche);
		free(muche);
		free(truc);
	}

	return 0;
}
