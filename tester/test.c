/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 23:56:44 by pbremond          #+#    #+#             */
/*   Updated: 2024/02/28 20:28:00 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include "libft.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>

int main()
{
	const char msg[] = "Hello world!\n";
	write(STDOUT_FILENO, msg, sizeof(msg));
	MALLOC(0);
	return 0;
}
