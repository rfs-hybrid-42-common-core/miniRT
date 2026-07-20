/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perlin.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 21:11:51 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:54:33 by maaugust         ###   ########.fr       */
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
 * @fn t_pattern perlin_pattern(t_color color_a, t_color color_b, int octaves)
 * @brief Initializes a raw Perlin/Fractal noise pattern.
 * @details Configures the parameters for a 3D Perlin noise generator. The 
 * 'octaves' parameter determines how many fractal layers are summed together 
 * to create finer, organic details.
 * @param color_a Primary color.
 * @param color_b Secondary color.
 * @param octaves Complexity level of the noise.
 * @return        The configured pattern struct.
 */
t_pattern	perlin_pattern(t_color color_a, t_color color_b, int octaves)
{
	t_pattern	pattern;
	t_matrix	identity;

	identity = identity_matrix();
	pattern = (t_pattern){PERLIN, color_a, color_b, identity, identity, {{0}}};
	pattern.u_data.s_noise.octaves = octaves;
	return (pattern);
}

/**
 * @fn t_color perlin_at(t_pattern *pattern, t_tuple pattern_point)
 * @brief Blends two colors based on raw fractal noise output.
 * @details Samples the fractal turbulence function at the given 3D point and 
 * maps the resulting scalar value [0.0, 1.0] to a linear interpolation between 
 * the two base colors.
 * @param pattern       The Perlin pattern.
 * @param pattern_point The local pattern coordinate.
 * @return              The interpolated color.
 */
t_color	perlin_at(t_pattern *pattern, t_tuple pattern_point)
{
	double	noise_value;

	noise_value = turbulence(pattern_point, pattern->u_data.s_noise.octaves);
	return (interpolation_color(pattern->color_a, pattern->color_b,
			noise_value));
}
