/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tbn_cube.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 01:33:10 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 21:29:17 by maaugust         ###   ########.fr       */
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
 * @fn static void get_cube_axes(t_tuple local_normal, t_tuple *tangent, 
 * t_tuple *bitangent)
 * @brief Determines the proper Tangent Space orientation for a cube face.
 * @details Evaluates the local normal to figure out which of the 6 faces was 
 * struck. It assigns hardcoded Tangent and Bitangent vectors to match standard 
 * cubical UV unwrapping orientation.
 * @param local_normal The surface normal in local object space.
 * @param tangent      Pointer to store the resulting Tangent vector.
 * @param bitangent    Pointer to store the resulting Bitangent vector.
 */
static void	get_cube_axes(t_tuple local_normal, t_tuple *tangent,
	t_tuple *bitangent)
{
	double	abs_x;
	double	abs_y;
	double	abs_z;

	abs_x = fabs(local_normal.x);
	abs_y = fabs(local_normal.y);
	abs_z = fabs(local_normal.z);
	if (abs_x >= abs_y && abs_x >= abs_z)
	{
		*tangent = vector(0, 0, -1);
		*bitangent = vector(0, 1, 0);
	}
	else if (abs_y >= abs_x && abs_y >= abs_z)
	{
		*tangent = vector(1, 0, 0);
		*bitangent = vector(0, 0, 1);
	}
	else
	{
		*tangent = vector(1, 0, 0);
		*bitangent = vector(0, 1, 0);
	}
}

/**
 * @fn t_tuple apply_cube_tbn(t_computations *comps, t_tuple map_normal)
 * @brief Applies a normal map to a unit Cube using a TBN matrix.
 * @details Pulls the geometric normal back into local space to figure out 
 * which flat side was hit. Retrieves the specific Tangent/Bitangent axes for 
 * that side, transforms them back into world space, and applies the 
 * perturbation matrix.
 * @param comps      Precomputed surface geometry.
 * @param map_normal The decoded vector from the normal map texture.
 * @return           The perturbed world-space normal.
 */
t_tuple	apply_cube_tbn(t_computations *comps, t_tuple map_normal)
{
	t_matrix	transposed;
	t_tuple		local_normal;
	t_tuple		tangent;
	t_tuple		bitangent;
	t_tuple		perturbed;

	transposed = transpose_matrix(&comps->object->transform);
	local_normal = multiply_matrix_tuple(&transposed, comps->normalv);
	get_cube_axes(local_normal, &tangent, &bitangent);
	tangent = normalize(multiply_matrix_tuple(&comps->object->transform,
				tangent));
	bitangent = normalize(multiply_matrix_tuple(&comps->object->transform,
				bitangent));
	perturbed.x = tangent.x * map_normal.x + bitangent.x * map_normal.y
		+ comps->normalv.x * map_normal.z;
	perturbed.y = tangent.y * map_normal.x + bitangent.y * map_normal.y
		+ comps->normalv.y * map_normal.z;
	perturbed.z = tangent.z * map_normal.x + bitangent.z * map_normal.y
		+ comps->normalv.z * map_normal.z;
	perturbed.w = 0.0;
	return (normalize(perturbed));
}
