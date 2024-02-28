/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 23:56:44 by pbremond          #+#    #+#             */
/*   Updated: 2024/02/28 02:29:26 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <libft.h>
#include <stdio.h>
#ifdef NDEBUG
# define FOO "release"
#else
# define FOO "debug"
#endif

int main()
{
	printf("Hello world! %s\n", FOO);
	return 0;
}
