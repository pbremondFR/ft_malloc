/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 23:56:44 by pbremond          #+#    #+#             */
/*   Updated: 2024/02/29 16:19:05 by pbremond         ###   ########.fr       */
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
	const char msg[] = "Hello world!\n";
	write(STDOUT_FILENO, msg, sizeof(msg));
	MALLOC(0);

	return 0;
}
