/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atoi_advance.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/21 18:52:20 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/21 20:51:16 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"
#include "parser.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static int get_sign(const char **nptr)
 * @brief Evaluates an optional sign character for an integer string.
 * @details Reads a '-' or '+' prefix and immediately advances the string 
 * pointer past it to streamline digit accumulation.
 * @param nptr Double pointer to the string.
 * @return     1 for positive, -1 for negative.
 */
static int	get_sign(const char **nptr)
{
	int	sign;

	sign = 1;
	if (**nptr == '+' || **nptr == '-')
	{
		if (**nptr == '-')
			sign = -1;
		(*nptr)++;
	}
	return (sign);
}

/**
 * @fn static void accumulate_digit(long *res, char c, int sign, bool *error)
 * @brief Constructs an integer mathematically digit by digit.
 * @details Multiplies the ongoing sum by 10 and adds the ASCII character's 
 * integer value. Crucially checks if the accumulated amount is actively 
 * exceeding the absolute limits of a 32-bit INT32_MAX, triggering a syntax 
 * error to prevent silent memory wrapping.
 * @param res   Pointer to the long accumulating the sum.
 * @param c     The specific ASCII character being evaluated.
 * @param sign  The positive/negative multiplier.
 * @param error Pointer to the boolean error flag.
 */
static void	accumulate_digit(long *res, char c, int sign, bool *error)
{
	*res = *res * 10 + (c - '0');
	if ((sign == 1 && *res > INT32_MAX)
		|| (sign == -1 && *res > (long)INT32_MAX + 1))
		*error = true;
}

/**
 * @fn int atoi_advance(const char **nptr, bool *error, bool comma_allowed)
 * @brief The core mathematical extraction engine for integers.
 * @details Validates the structure, converts the ASCII block, detects 
 * overflow, and actively updates the primary string pointer to point to the 
 * next block of data in the sequence. Exits immediately if the global error 
 * flag is triggered.
 * @param nptr          Double pointer to the raw string.
 * @param error         Pointer to a boolean flag that halts parsing upon 
 * failure.
 * @param comma_allowed True if the number expects an adjoining comma.
 * @return              The correctly scaled 32-bit integer.
 */
int	atoi_advance(const char **nptr, bool *error, bool comma_allowed)
{
	long	res;
	int		sign;
	bool	has_digits;

	if (*error)
		return (0);
	res = 0;
	has_digits = false;
	while (ft_isspace(**nptr))
		(*nptr)++;
	sign = get_sign(nptr);
	while (ft_isdigit(**nptr))
	{
		has_digits = true;
		accumulate_digit(&res, **nptr, sign, error);
		if (*error)
			return (0);
		(*nptr)++;
	}
	if (!has_digits || (**nptr && !ft_isspace(**nptr)
			&& (!comma_allowed || **nptr != ',')))
		*error = true;
	return ((int)(res * sign));
}
