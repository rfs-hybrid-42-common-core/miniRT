/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checkers.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 03:19:43 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:52:20 by maaugust         ###   ########.fr       */
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
 * @fn t_pattern checkers_pattern(t_color color_a, t_color color_b)
 * @brief Initializes a 3D procedural checkerboard pattern.
 * @details Creates a volumetric checker pattern that maps directly to 
 * 3D spatial coordinates. The transformation matrices are initialized 
 * to identity, allowing  the pattern to be independently scaled, rotated, 
 * or translated later.
 * @param color_a Primary checker color.
 * @param color_b Secondary checker color.
 * @return        The configured pattern struct.
 */
t_pattern	checkers_pattern(t_color color_a, t_color color_b)
{
	t_matrix	identity;

	identity = identity_matrix();
	return ((t_pattern){CHECKER, color_a, color_b, identity, identity, {{0}}});
}

/**
 * @fn t_color checkers_at(t_pattern *pattern, t_tuple pattern_point)
 * @brief Samples a 3D checkerboard at a given coordinate.
 * @details Sums the floored X, Y, and Z coordinates. If the sum is even, 
 * returns color A; if odd, returns color B. EPSILON is added to prevent 
 * floating-point precision artifacts at exactly 0.0 boundary lines.
 * @param pattern       The checker pattern.
 * @param pattern_point The local pattern coordinate.
 * @return              The evaluated color.
 */
t_color	checkers_at(t_pattern *pattern, t_tuple pattern_point)
{
	if ((int)(floor(pattern_point.x + EPSILON)
		+ floor(pattern_point.y + EPSILON)
		+ floor(pattern_point.z + EPSILON)) % 2 == 0)
		return (pattern->color_a);
	return (pattern->color_b);
}
