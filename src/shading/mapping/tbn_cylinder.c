/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tbn_cylinder.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 01:27:46 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 21:27:51 by maaugust         ###   ########.fr       */
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
 * @fn t_tuple apply_cylinder_tbn(t_computations *comps, t_tuple map_normal)
 * @brief Applies a normal map to a Cylinder using a TBN matrix.
 * @details Evaluates the local Up and Right vectors cross products to 
 * dynamically wrap the Tangent Space around the curved walls and flat caps of 
 * the cylinder.
 * @param comps      Precomputed surface geometry.
 * @param map_normal The decoded vector from the normal map texture.
 * @return           The perturbed world-space normal.
 */
t_tuple	apply_cylinder_tbn(t_computations *comps, t_tuple map_normal)
{
	t_tuple	local_up;
	t_tuple	local_right;
	t_tuple	tangent;
	t_tuple	bitangent;
	t_tuple	perturbed;

	local_up = normalize(multiply_matrix_tuple(&comps->object->transform,
				vector(0, 1, 0)));
	local_right = normalize(multiply_matrix_tuple(&comps->object->transform,
				vector(1, 0, 0)));
	if (fabs(dot(comps->normalv, local_up)) > 1.0 - EPSILON)
		tangent = normalize(cross(comps->normalv, local_right));
	else
		tangent = normalize(cross(local_up, comps->normalv));
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
