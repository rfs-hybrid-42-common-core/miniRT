/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atof_advance.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/21 15:23:24 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/21 21:06:18 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"
#include "parser.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stddef.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static bool skip_digits(const char **nptr)
 * @brief Validates and advances the pointer past a continuous block of digits.
 * @details Helper function that returns true if at least one valid numeric 
 * digit was found and bypassed.
 * @param nptr Double pointer to the actively evaluated string character.
 * @return     True if digits were present, false otherwise.
 */
static bool	skip_digits(const char **nptr)
{
	bool	found;

	found = false;
	while (ft_isdigit(**nptr))
	{
		found = true;
		(*nptr)++;
	}
	return (found);
}

/**
 * @fn static bool validate_float_structure(const char **nptr)
 * @brief Checks if a string exactly conforms to standard floating-point syntax.
 * @details Evaluates the presence of optional signs (+/-), integer blocks, 
 * decimal periods, fractional blocks, and Scientific Notation (e/E) 
 * structures. Prevents the parser from swallowing mathematically ambiguous 
 * strings.
 * @param nptr Double pointer to the string being evaluated.
 * @return     True if the entire sequence is geometrically valid.
 */
static bool	validate_float_structure(const char **nptr)
{
	bool	has_digits;

	if (**nptr == '+' || **nptr == '-')
		(*nptr)++;
	has_digits = skip_digits(nptr);
	if (**nptr == '.')
	{
		(*nptr)++;
		if (!skip_digits(nptr))
			return (false);
		has_digits = true;
	}
	if (!has_digits)
		return (false);
	if (**nptr == 'e' || **nptr == 'E')
	{
		(*nptr)++;
		if (**nptr == '+' || **nptr == '-')
			(*nptr)++;
		if (!skip_digits(nptr))
			return (false);
	}
	return (true);
}

/**
 * @fn double atof_advance(const char **nptr, bool *error, bool comma_allowed)
 * @brief The core mathematical extraction engine for floats.
 * @details Integrates structure validation, buffer overrun checks 
 * (MAX_FLOAT_LEN), 
 * and actual conversion (via libft's ft_atof). It then shifts the original 
 * string pointer to the exact character immediately following the number, 
 * completely 
 * negating the need to tokenize strings or use expensive string duplications.
 * @param nptr          Double pointer to the raw string.
 * @param error         Pointer to a boolean flag that halts parsing upon 
 * failure.
 * @param comma_allowed True if the number expects an adjoining comma (like a 
 * tuple).
 * @return              The fully evaluated double-precision floating point 
 * number.
 */
double	atof_advance(const char **nptr, bool *error, bool comma_allowed)
{
	double		res;
	const char	*start;

	if (*error)
		return (0.0);
	while (ft_isspace(**nptr))
		(*nptr)++;
	start = *nptr;
	if (!validate_float_structure(nptr)
		|| (size_t)(*nptr - start) > MAX_FLOAT_LEN)
	{
		*error = true;
		return (0.0);
	}
	res = ft_atof(start);
	if (**nptr && !ft_isspace(**nptr) && (!comma_allowed || **nptr != ','))
		*error = true;
	return (res);
}
