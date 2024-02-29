/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 23:56:44 by pbremond          #+#    #+#             */
/*   Updated: 2024/02/29 23:21:55 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include "libft.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

__attribute_maybe_unused__
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
	int pagesize1 = getpagesize();
	long pagesize2 = sysconf(_SC_PAGESIZE);
	dbg_print("Page size: %d, %ld\n", pagesize1, pagesize2);

	const char msg[] = "Hello world!\n";
	write(STDOUT_FILENO, msg, sizeof(msg));
	char *test = MALLOC(sizeof(msg));
	void *freeme = test;
	strcpy(test, msg);
	dbg_print("%s", test + 4096);
	int i = 8;
	while (i < 999999999)
	{
		dbg_print("%d - %u\n", i++, *(test++));
		// write(STDOUT_FILENO, (test++) + '0', 1);
	}
	FREE(freeme);
	return 0;
}
