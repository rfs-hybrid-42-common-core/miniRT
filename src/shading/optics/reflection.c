/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reflection.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 15:18:13 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 17:57:37 by maaugust         ###   ########.fr       */
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
 * @fn t_color reflected_color(t_world *world, t_computations *comps, 
 * int remaining)
 * @brief Spawns a secondary ray to sample mirrored environments.
 * @details Exits early if the material's reflectivity is zero or the recursive 
 * bounce limit has been reached. Scales the sampled environment color by the 
 * material's reflective strength.
 * @param world     The global world state.
 * @param comps     Precomputed surface geometry and reflection vector.
 * @param remaining Depth counter preventing infinite recursion (mirrors facing 
 * mirrors).
 * @return          The evaluated reflection color contribution.
 */
t_color	reflected_color(t_world *world, t_computations *comps, int remaining)
{
	t_ray	reflect_ray;

	if (remaining <= 0 || comps->object->material.reflective < EPSILON)
		return (color(0, 0, 0));
	reflect_ray = create_ray(comps->over_point, comps->reflectv, comps->time);
	return (scaling_color(color_at(world, &reflect_ray, remaining - 1),
			comps->object->material.reflective));
}
