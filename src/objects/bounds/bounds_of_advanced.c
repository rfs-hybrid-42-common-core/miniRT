/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bounds_of_advanced.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 16:01:04 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 04:42:32 by maaugust         ###   ########.fr       */
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
 * @fn t_bounds bounds_of_torus(t_object *object)
 * @brief Calculates the bounding box for a torus.
 * @details The bounding box spans the major radius plus the minor radius along 
 * the X and Z axes, and just the minor radius along the Y axis (the tube 
 * height).
 * @param object The torus object.
 * @return       The bounding box.
 */
t_bounds	bounds_of_torus(t_object *object)
{
	double	limit_x;
	double	limit_y;
	double	limit_z;

	limit_x = object->data.torus.major_r + object->data.torus.minor_r;
	limit_y = object->data.torus.minor_r;
	limit_z = limit_x;
	return ((t_bounds){point(-limit_x, -limit_y, -limit_z),
		point(limit_x, limit_y, limit_z)});
}

/**
 * @fn t_bounds bounds_of_triangle(t_object *object)
 * @brief Calculates the bounding box encapsulating a triangle's 3 vertices.
 * @details Starts with an empty bounding box and sequentially expands it to 
 * include each of the three absolute spatial coordinates of the triangle.
 * @param object The triangle object.
 * @return       The bounding box.
 */
t_bounds	bounds_of_triangle(t_object *object)
{
	t_bounds	box;

	box = empty_bounding_box();
	add_point_to_bounds(&box, object->data.triangle.p[0]);
	add_point_to_bounds(&box, object->data.triangle.p[1]);
	add_point_to_bounds(&box, object->data.triangle.p[2]);
	return (box);
}

/**
 * @fn t_bounds bounds_of_smooth_triangle(t_object *object)
 * @brief Calculates the bounding box encapsulating a smooth triangle.
 * @details Functions identically to a standard triangle, wrapping the spatial 
 * coordinates of the 3 vertices and mathematically ignoring the vertex normals.
 * @param object The smooth triangle object.
 * @return       The bounding box.
 */
t_bounds	bounds_of_smooth_triangle(t_object *object)
{
	t_bounds	box;

	box = empty_bounding_box();
	add_point_to_bounds(&box, object->data.smooth_triangle.p[0]);
	add_point_to_bounds(&box, object->data.smooth_triangle.p[1]);
	add_point_to_bounds(&box, object->data.smooth_triangle.p[2]);
	return (box);
}

/**
 * @fn t_bounds bounds_of_group(t_object *object)
 * @brief Retrieves the precalculated bounding box of a group.
 * @details Groups dynamically update their unified bounding box whenever a new 
 * child is added. This function simply returns that cached AABB without 
 * needing to recalculate the hierarchy.
 * @param object The group object.
 * @return       The bounding box.
 */
t_bounds	bounds_of_group(t_object *object)
{
	return (object->data.group.bounds);
}

/**
 * @fn t_bounds bounds_of_csg(t_object *object)
 * @brief Calculates the bounding box of a CSG boolean operation.
 * @details Highly optimized. DIFFERENCE returns only the left bounds. 
 * INTERSECTION physically calculates the 3D overlap, tightening the BVH to 
 * avoid false-positive ray tests.
 * @param object The CSG node.
 * @return       The resulting bounding box.
 */
t_bounds	bounds_of_csg(t_object *object)
{
	t_bounds	left_box;
	t_bounds	right_box;
	t_bounds	intersection_box;

	left_box = parent_space_bounds_of(object->data.csg.left);
	if (object->data.csg.operation == CSG_DIFFERENCE)
		return (left_box);
	right_box = parent_space_bounds_of(object->data.csg.right);
	if (object->data.csg.operation == CSG_INTERSECTION)
	{
		intersection_box.min.x = get_fmax(left_box.min.x, right_box.min.x);
		intersection_box.min.y = get_fmax(left_box.min.y, right_box.min.y);
		intersection_box.min.z = get_fmax(left_box.min.z, right_box.min.z);
		intersection_box.max.x = get_fmin(left_box.max.x, right_box.max.x);
		intersection_box.max.z = get_fmin(left_box.max.z, right_box.max.z);
		intersection_box.max.y = get_fmin(left_box.max.y, right_box.max.y);
		if (intersection_box.min.x > intersection_box.max.x
			|| intersection_box.min.y > intersection_box.max.y
			|| intersection_box.min.z > intersection_box.max.z)
			return (empty_bounding_box());
		return (intersection_box);
	}
	add_box_to_bounds(&left_box, &right_box);
	return (left_box);
}
