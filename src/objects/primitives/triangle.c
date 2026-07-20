/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   triangle.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 15:52:10 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:22:09 by maaugust         ###   ########.fr       */
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
 * @fn t_tuple normal_triangle(t_intersection *hit, t_tuple object_point)
 * @brief Retrieves the surface normal for a standard triangle.
 * @details Simply returns the face normal that was aggressively precomputed 
 * during the parsing phase.
 * @param hit          The intersection data.
 * @param object_point The point on the surface (unused).
 * @return             The constant geometric surface normal.
 */
t_tuple	normal_triangle(t_intersection *hit, t_tuple object_point)
{
	(void)object_point;
	return (hit->object->data.triangle.normal);
}

/**
 * @fn t_tuple normal_smooth_triangle(t_intersection *hit, t_tuple object_point)
 * @brief Interpolates the surface normal for a smooth triangle.
 * @details Extracts the Barycentric coordinates (u, v) captured during the 
 * Möller–Trumbore intersection algorithm and uses them to smoothly interpolate 
 * between the 3 explicit vertex normals. 
 * @param hit          The intersection data containing u/v coordinates.
 * @param object_point The point on the surface (unused).
 * @return             The interpolated, Phong-shaded surface normal.
 */
t_tuple	normal_smooth_triangle(t_intersection *hit, t_tuple object_point)
{
	t_smooth_triangle	*triangle;
	t_tuple				n[3];

	(void)object_point;
	triangle = &hit->object->data.smooth_triangle;
	n[0] = scaling_tuple(triangle->n[0], 1 - hit->u - hit->v);
	n[1] = scaling_tuple(triangle->n[1], hit->u);
	n[2] = scaling_tuple(triangle->n[2], hit->v);
	return (normalize(add_tuples(n[0], add_tuples(n[1], n[2]))));
}

/**
 * @fn void intersect_triangle(t_intersections *xs, t_ray *ray, 
 * t_object *object)
 * @brief Calculates ray intersections with a triangle.
 * @details Implements the highly optimized Möller–Trumbore intersection 
 * algorithm. Bypasses standard plane derivation entirely. Directly injects 
 * the Barycentric u and v coordinates into the intersection struct for later 
 * shading.
 * @param xs     Pointer to the intersections container.
 * @param ray    The transformed local-space ray.
 * @param object The triangle being tested.
 */
void	intersect_triangle(t_intersections *xs, t_ray *ray, t_object *object)
{
	t_tuple	vec[3];
	double	det;
	double	f;
	double	val[3];

	vec[0] = cross(ray->direction, object->data.triangle.e[1]);
	det = dot(object->data.triangle.e[0], vec[0]);
	if (fabs(det) < EPSILON)
		return ;
	f = 1.0 / det;
	vec[1] = subtract_tuples(ray->origin, object->data.triangle.p[0]);
	val[0] = f * dot(vec[1], vec[0]);
	if (val[0] < -EPSILON || val[0] > 1.0 + EPSILON)
		return ;
	vec[2] = cross(vec[1], object->data.triangle.e[0]);
	val[1] = f * dot(ray->direction, vec[2]);
	if (val[1] < -EPSILON || val[0] + val[1] > 1.0 + EPSILON)
		return ;
	val[2] = f * dot(object->data.triangle.e[1], vec[2]);
	if (xs->count < MAX_INTERSECTIONS)
	{
		xs->array[xs->count] = (t_intersection){val[0], val[1], val[2], object};
		xs->count++;
	}
}
