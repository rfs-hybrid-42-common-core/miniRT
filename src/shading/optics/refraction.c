/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   refraction.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 15:18:56 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 17:58:19 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "shading.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static t_tuple get_refract_direction(t_computations *comps, 
 * double n_ratio, double cos_i, double cos_t)
 * @brief Mathematically applies Snell's Law to bend a ray.
 * @details Combines the surface normal and the incoming eye vector using the 
 * calculated trigonometric limits to find the exact trajectory light will take 
 * passing through the material density barrier.
 * @param comps   Precomputed geometric data.
 * @param n_ratio The divided ratio of n1 / n2.
 * @param cos_i   The cosine of the incident angle.
 * @param cos_t   The cosine of the transmission angle.
 * @return        The correctly bent transmission vector.
 */
static t_tuple	get_refract_direction(t_computations *comps, double n_ratio,
	double cos_i, double cos_t)
{
	t_tuple	normal_scaled;
	t_tuple	eyev_scaled;

	normal_scaled = scaling_tuple(comps->normalv, n_ratio * cos_i - cos_t);
	eyev_scaled = scaling_tuple(comps->eyev, n_ratio);
	return (subtract_tuples(normal_scaled, eyev_scaled));
}

/**
 * @fn t_color refracted_color(t_world *world, t_computations *comps, 
 * int remaining)
 * @brief Spawns a ray to sample light passing through a transparent object.
 * @details Evaluates the sin2_t constraint. If sin2_t exceeds 1.0, light 
 * cannot physically escape the material (Total Internal Reflection), and the 
 * function returns black (deferring entirely to the mirror reflection pass).
 * @param world     The global world state.
 * @param comps     Precomputed surface geometry.
 * @param remaining Depth counter preventing infinite recursive bouncing.
 * @return          The filtered transmitted color.
 */
t_color	refracted_color(t_world *world, t_computations *comps, int remaining)
{
	t_ray	refract_ray;
	double	n_ratio;
	double	cos_i;
	double	cos_t;
	double	sin2_t;

	if (remaining <= 0 || comps->object->material.transparency < EPSILON)
		return (color(0, 0, 0));
	n_ratio = comps->n1 / comps->n2;
	cos_i = dot(comps->eyev, comps->normalv);
	sin2_t = n_ratio * n_ratio * (1.0 - cos_i * cos_i);
	if (sin2_t > 1.0 + EPSILON)
		return (color(0, 0, 0));
	cos_t = sqrt(1.0 - sin2_t);
	refract_ray = create_ray(comps->under_point,
			get_refract_direction(comps, n_ratio, cos_i, cos_t), comps->time);
	return (scaling_color(color_at(world, &refract_ray, remaining - 1),
			comps->object->material.transparency));
}
