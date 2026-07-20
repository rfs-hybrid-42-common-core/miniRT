/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ring.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 03:11:05 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:58:30 by maaugust         ###   ########.fr       */
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
 * @fn t_pattern ring_pattern(t_color color_a, t_color color_b)
 * @brief Initializes a concentric ring procedural pattern.
 * @details Creates a volumetric pattern that radiates outwards from the local 
 * origin in concentric cylinders (based on X and Z axes). The transformation 
 * matrices are initialized to identity, allowing independent scaling and 
 * rotation.
 * @param color_a Primary ring color.
 * @param color_b Secondary ring color.
 * @return        The configured pattern struct.
 */
t_pattern	ring_pattern(t_color color_a, t_color color_b)
{
	t_matrix	identity;

	identity = identity_matrix();
	return ((t_pattern){RING, color_a, color_b, identity, identity, {{0}}});
}

/**
 * @fn t_color ring_at(t_pattern *pattern, t_tuple pattern_point)
 * @brief Samples a ring pattern based on distance from the Y axis.
 * @details Uses the Pythagorean theorem on the X and Z axes. Floors the 
 * radius to create solid bands of alternating colors.
 * @param pattern       The ring pattern.
 * @param pattern_point The local pattern coordinate.
 * @return              The evaluated color.
 */
t_color	ring_at(t_pattern *pattern, t_tuple pattern_point)
{
	if ((int)floor(sqrt(pattern_point.x * pattern_point.x
				+ pattern_point.z * pattern_point.z) + EPSILON) % 2 == 0)
		return (pattern->color_a);
	return (pattern->color_b);
}
