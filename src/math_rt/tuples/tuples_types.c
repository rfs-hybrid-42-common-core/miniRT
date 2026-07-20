/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tuples_types.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 23:42:16 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:37:28 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "math_rt.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn t_tuple point(double x, double y, double z)
 * @brief Creates a 3D point tuple.
 * @details Sets the homogeneous w-coordinate to 1.0, ensuring the tuple can be 
 * translated (moved) through space by transformation matrices.
 * @param x The X coordinate.
 * @param y The Y coordinate.
 * @param z The Z coordinate.
 * @return  A point tuple.
 */
t_tuple	point(double x, double y, double z)
{
	return ((t_tuple){x, y, z, 1.0});
}

/**
 * @fn t_tuple vector(double x, double y, double z)
 * @brief Creates a 3D vector tuple.
 * @details Sets the homogeneous w-coordinate to 0.0. Vectors represent 
 * direction and magnitude, and mathematically ignore spatial translations.
 * @param x The X direction.
 * @param y The Y direction.
 * @param z The Z direction.
 * @return  A vector tuple.
 */
t_tuple	vector(double x, double y, double z)
{
	return ((t_tuple){x, y, z, 0.0});
}

/**
 * @fn t_color color(double r, double g, double b)
 * @brief Creates a floating-point RGB color.
 * @details Maps colors from 0.0 to 1.0. This allows for accurate mathematical 
 * blending (e.g., halving the light intensity by multiplying by 0.5).
 * @param r Red value (0.0 to 1.0).
 * @param g Green value (0.0 to 1.0).
 * @param b Blue value (0.0 to 1.0).
 * @return  A color struct.
 */
t_color	color(double r, double g, double b)
{
	return ((t_color){r, g, b});
}
