/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   marble.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 21:11:56 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:54:45 by maaugust         ###   ########.fr       */
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
 * @fn t_pattern marble_pattern(t_color color_a, t_color color_b, int octaves, 
 * double vein_scale)
 * @brief Initializes a procedural marble texture generator.
 * @details Sets up a texture that utilizes a sine wave perturbed by layered 
 * Perlin noise (turbulence) to physically simulate natural stone veins.
 * @param color_a    Base stone color.
 * @param color_b    Vein color.
 * @param octaves    Complexity level of the noise perturbation.
 * @param vein_scale Density and scaling of the marble veins.
 * @return           The configured pattern struct.
 */
t_pattern	marble_pattern(t_color color_a, t_color color_b, int octaves,
	double vein_scale)
{
	t_pattern	pattern;
	t_matrix	identity;

	identity = identity_matrix();
	pattern = (t_pattern){MARBLE, color_a, color_b, identity, identity, {{0}}};
	pattern.u_data.s_noise.octaves = octaves;
	pattern.u_data.s_noise.vein_scale = vein_scale;
	return (pattern);
}

/**
 * @fn t_color marble_at(t_pattern *pattern, t_tuple pattern_point)
 * @brief Simulates marble veins using a noise-perturbed sine wave.
 * @details Generates a pure sine wave gradient along the X-axis, but injects 
 * fractal turbulence into the phase of the wave, warping it organically.
 * @param pattern       The marble pattern.
 * @param pattern_point The local pattern coordinate.
 * @return              The evaluated color.
 */
t_color	marble_at(t_pattern *pattern, t_tuple pattern_point)
{
	double	noise_value;

	noise_value = turbulence(pattern_point, pattern->u_data.s_noise.octaves);
	noise_value = fabs(sin(pattern_point.x * 2.0 + noise_value
				* pattern->u_data.s_noise.vein_scale));
	return (interpolation_color(pattern->color_a, pattern->color_b,
			noise_value));
}
