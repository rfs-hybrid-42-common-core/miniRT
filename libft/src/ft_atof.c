/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 22:21:56 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/14 22:32:11 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stdbool.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

static double	parse_exponent(const char **nptr)
{
	double	exp;
	bool	neg;
	int		power;

	exp = 1.0;
	neg = false;
	power = 0;
	(*nptr)++;
	if (**nptr == '+' || **nptr == '-')
	{
		neg = (**nptr == '-');
		(*nptr)++;
	}
	while (ft_isdigit(**nptr))
	{
		power = power * 10 + (**nptr - '0');
		(*nptr)++;
	}
	while (power-- > 0)
		exp *= 10.0;
	if (neg)
		return (1.0 / exp);
	return (exp);
}

/**
 * @fn static double parse_fraction(const char **nptr)
 * @brief Parses the fractional part of a double.
 * @param nptr Pointer to the string right after the decimal point.
 * @return     The parsed fractional value.
 */
static double	parse_fractional(const char **nptr)
{
	double	frac;
	double	div;

	frac = 0.0;
	div = 1.0;
	while (ft_isdigit(**nptr))
	{
		frac = frac * 10.0 + (**nptr - '0');
		div *= 10.0;
		(*nptr)++;
	}
	return (frac / div);
}

/**
 * @fn double ft_atof(const char *nptr)
 * @brief Converts an ASCII string to a double-precision floating-point number.
 * @param nptr The string to parse.
 * @return     The parsed double value.
 */
double	ft_atof(const char *nptr)
{
	double	res;
	int		sign;

	res = 0.0;
	sign = 1;
	while (ft_isspace(*nptr))
		nptr++;
	if (*nptr == '+' || *nptr == '-')
	{
		sign = (*nptr == '+') - (*nptr == '-');
		nptr++;
	}
	while (ft_isdigit(*nptr))
	{
		res = res * 10.0 + (*nptr - '0');
		nptr++;
	}
	if (*nptr == '.')
	{
		nptr++;
		res = res + parse_fractional(&nptr);
	}
	if (*nptr == 'e' || *nptr == 'E')
		res *= parse_exponent(&nptr);
	return (res * sign);
}
