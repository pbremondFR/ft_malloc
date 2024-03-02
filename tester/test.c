/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 23:56:44 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/02 18:29:00 by pbremond         ###   ########.fr       */
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

__attribute__((unused))
static void	do_static_asserts()
{
	static_assert(ALIGN(0x123) % alignof(void*) == 0, "Incorrect alignment macro");
	static_assert(ALIGN(0x2) % alignof(void*) == 0, "Incorrect alignment macro");
	static_assert(ALIGN(0x0) % alignof(void*) == 0, "Incorrect alignment macro");

	static_assert(ALIGN_TO(0x123, 4) % 4 == 0, "Incorrect alignment macro");
	static_assert(ALIGN_TO(0x2, 4) % 4 == 0, "Incorrect alignment macro");
	static_assert(ALIGN_TO(0x0, 4) % 4 == 0, "Incorrect alignment macro");

	static_assert(ALIGN_TO(0x123, 16) % 16 == 0, "Incorrect alignment macro");
	static_assert(ALIGN_TO(0x2, 16) % 16 == 0, "Incorrect alignment macro");
	static_assert(ALIGN_TO(0x0, 16) % 16 == 0, "Incorrect alignment macro");
}

int main()
{
	dbg_print("Page size: %d, %ld\n", getpagesize(), sysconf(_SC_PAGESIZE));

	const char msg[] = "Hello world!\n";
	write(STDOUT_FILENO, msg, sizeof(msg));
	char *test = MALLOC(sizeof(msg));
	void *freeme = test;
	strcpy(test, msg);
	dbg_print("%s", test);
#ifndef NDEBUG
	int i = 8;
	while (i < 999999999) // Pages are indeed 4096 bytes long
		dbg_print("%d - %u\n", i++, *(test++));
#endif
	FREE(freeme);
	return 0;
}
