/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 00:35:08 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:18:12 by maaugust         ###   ########.fr       */
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
 * @fn t_tuple normal_cylinder(t_intersection *hit, t_tuple object_point)
 * @brief Calculates the surface normal for a cylinder.
 * @details Checks if the hit point rests on the flat top/bottom caps. If so, 
 * returns a vertical normal. Otherwise, returns a normal perpendicular to the 
 * curved cylindrical wall (ignoring the Y component).
 * @param hit          The intersection data.
 * @param object_point The point on the surface in local space.
 * @return             The geometric surface normal.
 */
t_tuple	normal_cylinder(t_intersection *hit, t_tuple object_point)
{
	double	dist;

	dist = object_point.x * object_point.x + object_point.z * object_point.z;
	if (dist < 1.0 + EPSILON)
	{
		if (object_point.y >= hit->object->data.cylinder.maximum - EPSILON)
			return (vector(0, 1, 0));
		if (object_point.y <= hit->object->data.cylinder.minimum + EPSILON)
			return (vector(0, -1, 0));
	}
	return (vector(object_point.x, 0, object_point.z));
}

/**
 * @fn static void intersect_caps(t_intersections *xs, t_ray *ray, 
 * t_object *object)
 * @brief Checks for intersections against the cylinder's end caps.
 * @details If the cylinder is marked "closed", evaluates if the ray strikes 
 * the mathematical planes at y=minimum and y=maximum within a radius of 1.0.
 * @param xs     Pointer to the intersections container.
 * @param ray    The transformed local-space ray.
 * @param object The cylinder being tested.
 */
static void	intersect_caps(t_intersections *xs, t_ray *ray, t_object *object)
{
	double	t;
	double	x;
	double	z;

	if (!object->data.cylinder.closed || fabs(ray->direction.y) < EPSILON)
		return ;
	t = (object->data.cylinder.minimum - ray->origin.y) / ray->direction.y;
	x = ray->origin.x + t * ray->direction.x;
	z = ray->origin.z + t * ray->direction.z;
	if (x * x + z * z <= 1.0 + EPSILON)
		add_intersection(xs, t, object);
	t = (object->data.cylinder.maximum - ray->origin.y) / ray->direction.y;
	x = ray->origin.x + t * ray->direction.x;
	z = ray->origin.z + t * ray->direction.z;
	if (x * x + z * z <= 1.0 + EPSILON)
		add_intersection(xs, t, object);
}

/**
 * @fn static void check_truncation(t_intersections *xs, t_ray *ray, 
 * t_object *object, double t)
 * @brief Validates if a calculated hit falls within the cylinder's height 
 * limits.
 * @details Solves for the exact Y coordinate of the hit. If it falls outside 
 * the defined min/max truncation limits, the intersection is discarded.
 * @param xs     Pointer to the intersections container.
 * @param ray    The transformed local-space ray.
 * @param object The cylinder being tested.
 * @param t      The calculated distance to test.
 */
static void	check_truncation(t_intersections *xs, t_ray *ray, t_object *object,
	double t)
{
	double	y;

	y = ray->origin.y + t * ray->direction.y;
	if (object->data.cylinder.minimum < y && y < object->data.cylinder.maximum)
		add_intersection(xs, t, object);
}

/**
 * @fn void intersect_cylinder(t_intersections *xs, t_ray *ray, 
 * t_object *object)
 * @brief Calculates ray intersections with a cylinder.
 * @details Computes the quadratic roots for an infinite cylinder tube. If the 
 * ray is perfectly parallel to the Y axis (coeffs[2] < EPSILON), the tube is 
 * missed entirely, but the caps might still be hit.
 * @param xs     Pointer to the intersections container.
 * @param ray    The transformed local-space ray.
 * @param object The cylinder being tested.
 */
void	intersect_cylinder(t_intersections *xs, t_ray *ray, t_object *object)
{
	double	coeffs[3];
	double	roots[2];
	int		total_roots;
	int		i;

	coeffs[2] = ray->direction.x * ray->direction.x
		+ ray->direction.z * ray->direction.z;
	if (coeffs[2] < EPSILON)
	{
		intersect_caps(xs, ray, object);
		return ;
	}
	coeffs[1] = 2.0 * (ray->origin.x * ray->direction.x
			+ ray->origin.z * ray->direction.z);
	coeffs[0] = ray->origin.x * ray->origin.x
		+ ray->origin.z * ray->origin.z - 1.0;
	total_roots = solve_quadratic(coeffs, roots);
	i = -1;
	while (++i < total_roots)
		check_truncation(xs, ray, object, roots[i]);
	intersect_caps(xs, ray, object);
}
