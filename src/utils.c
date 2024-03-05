/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 20:23:25 by pbremond          #+#    #+#             */
/*   Updated: 2024/03/05 20:26:51 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "libft.h"

void	rec_putnbr(int n)
{
	unsigned int	u_nb;

	if (n < 0)
	{
		u_nb = n * -1;
		write(STDOUT_FILENO, "-", 1);
	}
	else
		u_nb = n;
	if (u_nb > 9)
	{
		rec_putnbr(u_nb / 10);
		rec_putnbr(u_nb % 10);
	}
	else
		ft_putchar(u_nb + '0');
}

// Takes a number in decimal form, and converts it to the given base.
// Base cannot contain '+', '-', whitespaces, or duplicating characters.
void	rec_putnbr_base(long nbr, const char *base)
{
	size_t	u_nb;
	size_t	base_len;

	base_len = ft_strlen(base);
	if (nbr < 0)
	{
		u_nb = nbr * -1;
		write(1, "-", 1);
	}
	else
		u_nb = nbr;
	if (u_nb >= base_len)
	{
		rec_putnbr_base(u_nb / base_len, base);
		write(1, &base[u_nb % base_len], 1);
	}
	else if (u_nb < base_len)
		write(1, &base[u_nb % base_len], 1);
}
