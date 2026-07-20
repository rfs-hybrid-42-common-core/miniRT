/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/01 18:34:03 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:15:37 by maaugust         ###   ########.fr       */
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
 * @fn t_tuple normal_plane(t_intersection *hit, t_tuple object_point)
 * @brief Returns the constant surface normal for a plane.
 * @details A local space plane always faces perfectly upwards along the Y 
 * axis. Any rotation applied to the plane is handled when this normal is 
 * converted back to world space.
 * @param hit          The intersection data (unused).
 * @param object_point The point on the surface (unused).
 * @return             The constant vector (0, 1, 0).
 */
t_tuple	normal_plane(t_intersection *hit, t_tuple object_point)
{
	(void)hit;
	(void)object_point;
	return (vector(0, 1, 0));
}

/**
 * @fn void intersect_plane(t_intersections *xs, t_ray *ray, t_object *object)
 * @brief Calculates ray intersections with a plane.
 * @details Since the plane spans the XZ axes at y=0, the intersection distance 
 * is simply the ratio of the ray's origin height to its vertical direction. 
 * Ray paths parallel to the plane return no intersection.
 * @param xs     Pointer to the intersections container.
 * @param ray    The transformed local-space ray.
 * @param object The plane being tested.
 */
void	intersect_plane(t_intersections *xs, t_ray *ray, t_object *object)
{
	double	t;

	if (fabs(ray->direction.y) < EPSILON)
		return ;
	t = -ray->origin.y / ray->direction.y;
	add_intersection(xs, t, object);
}
