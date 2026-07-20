/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color_arithmetics.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 03:34:07 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/02 00:28:06 by maaugust         ###   ########.fr       */
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
 * @fn t_color add_colors(t_color c1, t_color c2)
 * @brief Adds two colors together.
 * @details Simulates the physical addition of light. Adding red (1,0,0) and 
 * green (0,1,0) yields yellow (1,1,0).
 * @param c1 The first color.
 * @param c2 The second color.
 * @return   The combined color.
 */
t_color	add_colors(t_color c1, t_color c2)
{
	return ((t_color){c1.r + c2.r, c1.g + c2.g, c1.b + c2.b});
}

/**
 * @fn t_color subtract_colors(t_color c1, t_color c2)
 * @brief Subtracts the second color from the first.
 * @details Removes specific color wavelengths from a base color.
 * @param c1 The base color.
 * @param c2 The color to subtract.
 * @return   The resultant color.
 */
t_color	subtract_colors(t_color c1, t_color c2)
{
	return ((t_color){c1.r - c2.r, c1.g - c2.g, c1.b - c2.b});
}

/**
 * @fn t_color multiply_colors(t_color c1, t_color c2)
 * @brief Multiplies two colors together (Hadamard Product).
 * @details Simulates a color filter or light reflecting off a colored surface. 
 * For example, a pure red light (1,0,0) hitting a pure blue surface (0,0,1) 
 * yields black (0,0,0) because the surface absorbs all the red light.
 * @param c1 The first color.
 * @param c2 The second color.
 * @return   The filtered color.
 */
t_color	multiply_colors(t_color c1, t_color c2)
{
	return ((t_color){c1.r * c2.r, c1.g * c2.g, c1.b * c2.b});
}

/**
 * @fn t_color scaling_color(t_color c, double n)
 * @brief Adjusts the intensity of a color.
 * @details Used to darken or brighten a color. E.g., multiplying by 0.5 halves 
 * the brightness.
 * @param c The base color.
 * @param n The scalar intensity multiplier.
 * @return  The scaled color.
 */
t_color	scaling_color(t_color c, double n)
{
	return ((t_color){c.r * n, c.g * n, c.b * n});
}

/**
 * @fn t_color interpolation_color(t_color a, t_color b, double t)
 * @brief Smoothly blends two colors together.
 * @details Uses a linear interpolation (LERP) formula based on parameter 't'. 
 * If t=0, returns color 'c1'. If t=1, returns color 'c2'. If t=0.5, returns a 
 * perfect 50/50 mix. Used heavily for gradients and procedural noise.
 * @param c1 The starting color.
 * @param c2 The ending color.
 * @param t  The blend factor (0.0 to 1.0).
 * @return   The interpolated color.
 */
t_color	interpolation_color(t_color c1, t_color c2, double t)
{
	return (add_colors(scaling_color(c1, 1.0 - t), scaling_color(c2, t)));
}
