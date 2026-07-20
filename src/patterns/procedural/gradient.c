/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gradient.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 17:02:58 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:53:06 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "patterns.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn t_pattern gradient_pattern(t_color color_a, t_color color_b)
 * @brief Initializes a linear procedural gradient pattern.
 * @details Creates a gradient that smoothly transitions between two colors 
 * strictly along the X-axis in local pattern space.
 * @param color_a Starting color.
 * @param color_b Ending color.
 * @return        The configured pattern struct.
 */
t_pattern	gradient_pattern(t_color color_a, t_color color_b)
{
	t_matrix	identity;

	identity = identity_matrix();
	return ((t_pattern){GRADIENT, color_a, color_b, identity, identity, {{0}}});
}

/**
 * @fn t_color gradient_at(t_pattern *pattern, t_tuple pattern_point)
 * @brief Samples a linear gradient along the X axis.
 * @details Uses the fractional part of the X coordinate to linearly interpolate 
 * (LERP) between color A and color B.
 * @param pattern       The gradient pattern.
 * @param pattern_point The local pattern coordinate.
 * @return              The blended color.
 */
t_color	gradient_at(t_pattern *pattern, t_tuple pattern_point)
{
	return (interpolation_color(pattern->color_a, pattern->color_b,
			pattern_point.x - floor(pattern_point.x)));
}
