/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tbn_triangle.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 23:14:22 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 21:30:07 by maaugust         ###   ########.fr       */
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
 * @fn static t_tuple core_triangle_tbn(t_tuple *e, t_tuple *uv, 
 * t_tuple normalv, t_tuple map_normal)
 * @brief Computes the TBN matrix using explicit triangle UV mapping data.
 * @details This is the industry-standard algorithm used by OpenGL/DirectX. It 
 * derives the Tangent vector by solving a system of equations correlating the 
 * physical 3D edge deltas (e) with the 2D texture UV deltas. Gram-Schmidt 
 * orthogonalization is applied to guarantee the Tangent perfectly aligns with 
 * the geometric face.
 * @param e          Array containing the 2 precomputed edge vectors.
 * @param uv         Array containing the 3 explicit UV mapping coordinates.
 * @param normalv    The interpolated or geometric surface normal.
 * @param map_normal The decoded vector from the normal map texture.
 * @return           The perturbed world-space normal.
 */
static t_tuple	core_triangle_tbn(t_tuple *e, t_tuple *uv, t_tuple normalv,
	t_tuple map_normal)
{
	t_tuple	delta_uv[2];
	t_tuple	tangent;
	t_tuple	bitangent;
	double	frac;
	t_tuple	perturbed;

	delta_uv[0] = subtract_tuples(uv[1], uv[0]);
	delta_uv[1] = subtract_tuples(uv[2], uv[0]);
	frac = 1.0 / (delta_uv[0].x * delta_uv[1].y
			- delta_uv[1].x * delta_uv[0].y);
	tangent.x = frac * (delta_uv[1].y * e[0].x - delta_uv[0].y * e[1].x);
	tangent.y = frac * (delta_uv[1].y * e[0].y - delta_uv[0].y * e[1].y);
	tangent.z = frac * (delta_uv[1].y * e[0].z - delta_uv[0].y * e[1].z);
	tangent.w = 0.0;
	tangent = normalize(subtract_tuples(tangent, scaling_tuple(normalv,
					dot(normalv, tangent))));
	bitangent = normalize(cross(normalv, tangent));
	perturbed.x = tangent.x * map_normal.x + bitangent.x * map_normal.y
		+ normalv.x * map_normal.z;
	perturbed.y = tangent.y * map_normal.x + bitangent.y * map_normal.y
		+ normalv.y * map_normal.z;
	perturbed.z = tangent.z * map_normal.x + bitangent.z * map_normal.y
		+ normalv.z * map_normal.z;
	perturbed.w = 0.0;
	return (normalize(perturbed));
}

/**
 * @fn t_tuple apply_triangle_tbn(t_computations *comps, t_tuple map_normal)
 * @brief Applies a normal map to a standard flat triangle.
 * @details Extracts the edge and UV parameters from the triangle struct and 
 * passes them to the core TBN algebraic solver.
 * @param comps      Precomputed surface geometry.
 * @param map_normal The decoded vector from the normal map texture.
 * @return           The perturbed world-space normal.
 */
t_tuple	apply_triangle_tbn(t_computations *comps, t_tuple map_normal)
{
	return (core_triangle_tbn(
			comps->object->data.triangle.e,
			comps->object->data.triangle.uv,
			comps->normalv,
			map_normal));
}

/**
 * @fn t_tuple apply_smooth_triangle_tbn(t_computations *comps, 
 * t_tuple map_normal)
 * @brief Applies a normal map to an interpolated smooth triangle.
 * @details Operates identically to the flat triangle, except the 
 * comps->normalv provided to the TBN solver has already been 
 * Phong-interpolated across the face using the Barycentric u/v coordinates.
 * @param comps      Precomputed surface geometry.
 * @param map_normal The decoded vector from the normal map texture.
 * @return           The perturbed world-space normal.
 */
t_tuple	apply_smooth_triangle_tbn(t_computations *comps, t_tuple map_normal)
{
	return (core_triangle_tbn(
			comps->object->data.smooth_triangle.e,
			comps->object->data.smooth_triangle.uv,
			comps->normalv,
			map_normal));
}
