/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_math.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/26 04:33:08 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 03:00:25 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "parser.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn t_tuple parse_tuple(const char **ptr, bool *error, double w)
 * @brief Extracts a 3D coordinate block.
 * @details Reads a structured `X,Y,Z` floating point sequence by chaining 
 * three `atof_advance` calls separated by mandatory comma checks.
 * @param ptr   Double pointer to the raw string.
 * @param error Pointer to the global error flag.
 * @param w     The homogeneous component (1.0 for points, 0.0 for vectors).
 * @return      The successfully parsed tuple.
 */
t_tuple	parse_tuple(const char **ptr, bool *error, double w)
{
	t_tuple	tuple;

	if (*error)
		return (vector(0, 0, 0));
	tuple.x = atof_advance(ptr, error, true);
	advance_comma(ptr, error);
	tuple.y = atof_advance(ptr, error, true);
	advance_comma(ptr, error);
	tuple.z = atof_advance(ptr, error, false);
	tuple.w = w;
	return (tuple);
}

/**
 * @fn t_color parse_color(const char **ptr, bool *error)
 * @brief Extracts an RGB color block.
 * @details Reads a structured `R,G,B` integer sequence. Enforces strict bounds 
 * checking to guarantee values fall exclusively between 0 and 255. Maps the 
 * final integer values down to internal floating-point floats (0.0 to 1.0).
 * @param ptr   Double pointer to the raw string.
 * @param error Pointer to the global error flag.
 * @return      The normalized, fractional color vector.
 */
t_color	parse_color(const char **ptr, bool *error)
{
	int	r;
	int	g;
	int	b;

	if (*error)
		return (color(0, 0, 0));
	r = atoi_advance(ptr, error, true);
	advance_comma(ptr, error);
	g = atoi_advance(ptr, error, true);
	advance_comma(ptr, error);
	b = atoi_advance(ptr, error, true);
	if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
		*error = true;
	return (color((double)r / 255.0, (double)g / 255.0, (double)b / 255.0));
}

/**
 * @fn t_tuple parse_normal(const char **ptr, bool *error)
 * @brief Extracts and validates an orientation vector.
 * @details Operates identically to `parse_tuple` but mathematically verifies 
 * that the X, Y, and Z parameters all sit within the absolute [-1.0, 1.0] 
 * range. Automatically normalizes the resulting vector before returning.
 * @param ptr   Double pointer to the raw string.
 * @param error Pointer to the global error flag.
 * @return      The validated unit normal vector.
 */
t_tuple	parse_normal(const char **ptr, bool *error)
{
	t_tuple	normal;

	normal = parse_tuple(ptr, error, 0.0);
	if (!validate_vector_range(normal) || magnitude(normal) < EPSILON)
		*error = true;
	return (normalize(normal));
}
