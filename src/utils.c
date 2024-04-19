/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 20:23:25 by pbremond          #+#    #+#             */
/*   Updated: 2024/04/19 18:51:59 by pbremond         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdbool.h>
#include "libft.h"

/*
 * No, I won't recode snprintf just for this stupid project, fuck you, have some
 * piscine functions.
 */

/*
 * Apparently it's hotly debated whether it's GCC's or glibc's fault that
 * we're supposed to check EVERY call to write. Even better, casting it to void
 * doesn't do SHIT. So either you have to precede every function call with (void)!
 * which is stupid and cluttery, or just yeet the warning out. So I'll just do that.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
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

void	rec_putsize_t(size_t n)
{
	if (n > 9)
	{
		rec_putsize_t(n / 10);
		rec_putsize_t(n % 10);
	}
	else
		ft_putchar(n + '0');
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
#pragma GCC diagnostic pop

void	tarace(const char * restrict fmt, ...)
{
	char buf[1024] = {0};
	va_list args;
	va_start(args, fmt);
	(void)vsnprintf(buf, sizeof(buf), fmt, args);
	ft_putstr(buf);
	va_end(args);
}

void	malloc_error(const char * restrict msg, bool abort_program)
{
	(void)abort_program;	// Avoid unused warning if in mandatory mode
	ft_putstr_fd("*** ft_malloc detected *** ", STDERR_FILENO);
	ft_putstr_fd(msg, STDERR_FILENO);
#ifdef FT_BONUS
	if (abort_program)
		abort();
#endif
}
