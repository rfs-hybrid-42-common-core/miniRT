/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   normal_at.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 00:34:06 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 13:40:46 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "objects.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn t_tuple world_to_object(t_object *object, t_tuple world_point, 
 * double time)
 * @brief Converts a 3D point from global world space into local object space.
 * @details Recursively transverses the object's parent group hierarchy, 
 * applying inverse transforms. Generates interpolated matrices for objects in 
 * motion.
 * @param object      The object containing the transform.
 * @param world_point The absolute world coordinate.
 * @param time        Temporal parameter for motion blur interpolation.
 * @return            The local object coordinate.
 */
t_tuple	world_to_object(t_object *object, t_tuple world_point,
	double time)
{
	t_matrix	interpolated;
	t_matrix	inverted;

	if (object->parent)
		world_point = world_to_object(object->parent, world_point, time);
	if (!object->is_moving)
		return (multiply_matrix_tuple(&object->transform_inverse, world_point));
	interpolated = interpolate_transform(&object->transform,
			&object->transform_end, time);
	inverted = inverse_matrix(&interpolated);
	return (multiply_matrix_tuple(&inverted, world_point));
}

/**
 * @fn static t_tuple normal_unsupported(t_intersection *hit, 
 * t_tuple object_point)
 * @brief Fallback for objects that cannot independently generate a normal.
 * @details Groups and CSG nodes are logical containers, not physical geometry, 
 * and therefore have no surface normal.
 * @param hit          The intersection data.
 * @param object_point The local object point.
 * @return             A blank (0,0,0) vector.
 */
static t_tuple	normal_unsupported(t_intersection *hit, t_tuple object_point)
{
	(void)hit;
	(void)object_point;
	return (vector(0, 0, 0));
}

/**
 * @fn static t_tuple object_normal_at(t_intersection *hit, 
 * t_tuple object_point)
 * @brief Routes to the correct normal calculator based on object type.
 * @details Uses a static function pointer jump table for fast, branchless 
 * execution during the shading loop.
 * @param hit          The intersection hit data.
 * @param object_point The calculated point in local space.
 * @return             The raw, local surface normal.
 */
static t_tuple	object_normal_at(t_intersection *hit, t_tuple object_point)
{
	static t_tuple	(*get_normal[])(t_intersection *, t_tuple) = {
	[SPHERE] = normal_sphere,
	[PLANE] = normal_plane,
	[CUBE] = normal_cube,
	[CYLINDER] = normal_cylinder,
	[CONE] = normal_cone,
	[TORUS] = normal_torus,
	[TRIANGLE] = normal_triangle,
	[SMOOTH_TRIANGLE] = normal_smooth_triangle,
	[GROUP] = normal_unsupported,
	[CSG] = normal_unsupported
	};

	if (hit->object->type < SPHERE || hit->object->type >= OBJECT_COUNT
		|| !get_normal[hit->object->type])
		return (vector(0, 0, 0));
	return (get_normal[hit->object->type](hit, object_point));
}

/**
 * @fn t_tuple normal_to_world(t_object *object, t_tuple normal, double time)
 * @brief Converts a local surface normal back into a true world space normal.
 * @details Crucially, this uses the Inverse-Transpose matrix. Simply 
 * multiplying a normal by a standard transform matrix will warp and skew the 
 * normal if the object was scaled non-uniformly. 
 * @param object The object containing the transform.
 * @param normal The raw local normal.
 * @param time   Temporal parameter for motion blur.
 * @return       The corrected, normalized world-space vector.
 */
t_tuple	normal_to_world(t_object *object, t_tuple normal, double time)
{
	t_matrix	interpolated;
	t_matrix	inverted;
	t_matrix	transposed;

	if (!object->is_moving)
		normal = multiply_matrix_tuple(&object->transform_inverse_transpose,
				normal);
	else
	{
		interpolated = interpolate_transform(&object->transform,
				&object->transform_end, time);
		inverted = inverse_matrix(&interpolated);
		transposed = transpose_matrix(&inverted);
		normal = multiply_matrix_tuple(&transposed, normal);
	}
	normal.w = 0.0;
	normal = normalize(normal);
	if (object->parent)
		normal = normal_to_world(object->parent, normal, time);
	return (normal);
}

/**
 * @fn t_tuple normal_at(t_intersection *hit, t_tuple world_point, double time)
 * @brief Master function to calculate the surface normal at an intersection.
 * @details Converts the point to local space, calculates the primitive's 
 * geometric normal, and safely converts it back to world space for shading.
 * @param hit         The intersection data.
 * @param world_point The exact intersection coordinate in global space.
 * @param time        Temporal parameter for motion blur.
 * @return            The final geometric surface normal.
 */
t_tuple	normal_at(t_intersection *hit, t_tuple world_point, double time)
{
	t_tuple		object_normal;
	t_tuple		object_point;

	object_point = world_to_object(hit->object, world_point, time);
	object_normal = object_normal_at(hit, object_point);
	return (normal_to_world(hit->object, object_normal, time));
}
