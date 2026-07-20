/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bounds_of_basic.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 16:00:34 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 13:55:44 by maaugust         ###   ########.fr       */
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
 * @fn t_bounds bounds_of_sphere(t_object *object)
 * @brief Returns the generic unit bounding box for a sphere.
 * @details Since the base sphere is defined at the origin with a radius of 
 * 1.0, its local bounding box always spans exactly from -1.0 to 1.0 on all 
 * axes. Scaling is handled purely by the transformation matrix.
 * @param object The sphere (unused natively as radius is standardized to 1.0).
 * @return       The unit bounding box.
 */
t_bounds	bounds_of_sphere(t_object *object)
{
	(void)object;
	return ((t_bounds){point(-1, -1, -1), point(1, 1, 1)});
}

/**
 * @fn t_bounds bounds_of_plane(t_object *object)
 * @brief Returns an infinite bounding box for a plane.
 * @details Thinly clamped on the Y axis to EPSILON to allow AABB hits.
 * @param object The plane.
 * @return       The pseudo-infinite bounding box.
 */
t_bounds	bounds_of_plane(t_object *object)
{
	(void)object;
	return ((t_bounds){point(-INFINITY, -EPSILON, -INFINITY),
		point(INFINITY, EPSILON, INFINITY)});
}

/**
 * @fn t_bounds bounds_of_cube(t_object *object)
 * @brief Returns the unit bounding box for a cube.
 * @details The base cube is an Axis-Aligned Bounding Box (AABB) centered at 
 * the origin, spanning from -1.0 to 1.0 on the X, Y, and Z axes.
 * @param object The cube.
 * @return       The unit bounding box.
 */
t_bounds	bounds_of_cube(t_object *object)
{
	(void)object;
	return ((t_bounds){point(-1, -1, -1), point(1, 1, 1)});
}

/**
 * @fn t_bounds bounds_of_cylinder(t_object *object)
 * @brief Returns the bounding box for a cylinder.
 * @details Factors in the dynamically truncated min/max Y limits.
 * @param object The cylinder.
 * @return       The dynamically scaled bounding box.
 */
t_bounds	bounds_of_cylinder(t_object *object)
{
	return ((t_bounds){point(-1, object->data.cylinder.minimum, -1),
		point(1, object->data.cylinder.maximum, 1)});
}

/**
 * @fn t_bounds bounds_of_cone(t_object *object)
 * @brief Returns the bounding box for a cone.
 * @details Cone width scales linearly with height, so the X/Z limits expand 
 * based on the absolute maximum extent of the Y limits.
 * @param object The cone.
 * @return       The bounding box.
 */
t_bounds	bounds_of_cone(t_object *object)
{
	double	limit;

	limit = get_fmax(fabs(object->data.cone.minimum),
			fabs(object->data.cone.maximum));
	return ((t_bounds){point(-limit, object->data.cone.minimum, -limit),
		point(limit, object->data.cone.maximum, limit)});
}
