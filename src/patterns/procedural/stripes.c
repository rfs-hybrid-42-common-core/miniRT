/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stripes.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/02 14:27:16 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:53:21 by maaugust         ###   ########.fr       */
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
 * @fn t_pattern stripe_pattern(t_color color_a, t_color color_b)
 * @brief Initializes a 1D alternating stripe procedural pattern.
 * @details Creates infinite parallel stripes that alternate based solely on 
 * the X-axis coordinate in local pattern space.
 * @param color_a Primary stripe color.
 * @param color_b Secondary stripe color.
 * @return        The configured pattern struct.
 */
t_pattern	stripe_pattern(t_color color_a, t_color color_b)
{
	t_matrix	identity;

	identity = identity_matrix();
	return ((t_pattern){STRIPE, color_a, color_b, identity, identity, {{0}}});
}

/**
 * @fn t_color stripe_at(t_pattern *pattern, t_tuple pattern_point)
 * @brief Samples a repeating stripe along the X axis.
 * @details Floors the X coordinate to toggle sharply between colors at integer 
 * boundaries.
 * @param pattern       The stripe pattern.
 * @param pattern_point The local pattern coordinate.
 * @return              The evaluated color.
 */
t_color	stripe_at(t_pattern *pattern, t_tuple pattern_point)
{
	if ((int)floor(pattern_point.x + EPSILON) % 2 == 0)
		return (pattern->color_a);
	return (pattern->color_b);
}
