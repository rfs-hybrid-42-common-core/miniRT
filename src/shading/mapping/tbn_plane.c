/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tbn_plane.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 01:24:52 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 21:28:43 by maaugust         ###   ########.fr       */
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
 * @fn t_tuple apply_plane_tbn(t_computations *comps, t_tuple map_normal)
 * @brief Applies a normal map to an infinite Plane using a TBN matrix.
 * @details Because a plane is natively flat along the XZ axes, its Tangent 
 * and Bitangent vectors are strictly bound to +X and +Z. Those vectors are 
 * transformed into world space, creating the TBN matrix to align the mapped 
 * normal.
 * @param comps      Precomputed surface geometry.
 * @param map_normal The decoded vector from the normal map texture.
 * @return           The perturbed world-space normal.
 */
t_tuple	apply_plane_tbn(t_computations *comps, t_tuple map_normal)
{
	t_tuple	tangent;
	t_tuple	bitangent;
	t_tuple	perturbed;

	tangent = normalize(multiply_matrix_tuple(&comps->object->transform,
				vector(1, 0, 0)));
	bitangent = normalize(multiply_matrix_tuple(&comps->object->transform,
				vector(0, 0, 1)));
	perturbed.x = tangent.x * map_normal.x + bitangent.x * map_normal.y
		+ comps->normalv.x * map_normal.z;
	perturbed.y = tangent.y * map_normal.x + bitangent.y * map_normal.y
		+ comps->normalv.y * map_normal.z;
	perturbed.z = tangent.z * map_normal.x + bitangent.z * map_normal.y
		+ comps->normalv.z * map_normal.z;
	perturbed.w = 0.0;
	return (normalize(perturbed));
}
