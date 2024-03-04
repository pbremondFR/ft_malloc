/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 23:56:44 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/04 16:45:40 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
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

int main()
{
	print_settings();
	printf("Thread id: %lu\n", pthread_self());

	const char msg[] = "Hello world!\n";
	write(STDOUT_FILENO, msg, sizeof(msg));
	char *test = MALLOC(sizeof(msg));
	void *freeme = test;
	strcpy(test, msg);
	dbg_print("%s", test);
#ifndef NDEBUG
	int i = 8;
	while (i < 10) // Pages are indeed 4096 bytes long
		dbg_print("%d - %u\n", i++, *(test++));
#endif
	FREE(freeme);
	return 0;
}
