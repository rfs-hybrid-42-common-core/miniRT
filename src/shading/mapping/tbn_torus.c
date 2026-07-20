/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tbn_torus.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 23:14:38 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 21:29:27 by maaugust         ###   ########.fr       */
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
 * @fn t_tuple apply_torus_tbn(t_computations *comps, t_tuple map_normal)
 * @brief Applies a normal map to a Torus using a TBN matrix.
 * @details Finds the Tangent by calculating the derivative vector along the 
 * major ring (a circle resting in the XZ plane). Uses the Gram-Schmidt process 
 * to strictly orthogonalize the Tangent against the geometric normal, ensuring 
 * the resulting TBN matrix is mathematically pure and unskewed.
 * @param comps      Precomputed surface geometry.
 * @param map_normal The decoded vector from the normal map texture.
 * @return           The perturbed world-space normal.
 */
t_tuple	apply_torus_tbn(t_computations *comps, t_tuple map_normal)
{
	t_tuple	object_point;
	t_tuple	tangent;
	t_tuple	bitangent;
	t_tuple	perturbed;

	object_point = world_to_object(comps->object, comps->over_point,
			comps->time);
	tangent = vector(-object_point.z, 0.0, object_point.x);
	if (magnitude(tangent) < EPSILON)
		tangent = vector(1.0, 0.0, 0.0);
	else
		tangent = normalize(tangent);
	tangent = normal_to_world(comps->object, tangent, comps->time);
	tangent = normalize(subtract_tuples(tangent, scaling_tuple(
					comps->normalv, dot(comps->normalv, tangent))));
	bitangent = normalize(cross(comps->normalv, tangent));
	perturbed.x = tangent.x * map_normal.x + bitangent.x * map_normal.y
		+ comps->normalv.x * map_normal.z;
	perturbed.y = tangent.y * map_normal.x + bitangent.y * map_normal.y
		+ comps->normalv.y * map_normal.z;
	perturbed.z = tangent.z * map_normal.x + bitangent.z * map_normal.y
		+ comps->normalv.z * map_normal.z;
	perturbed.w = 0.0;
	return (normalize(perturbed));
}
