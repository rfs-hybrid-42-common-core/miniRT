/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cone.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 03:42:45 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:27:21 by maaugust         ###   ########.fr       */
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
 * @fn t_tuple normal_cone(t_intersection *hit, t_tuple object_point)
 * @brief Calculates the surface normal for a cone.
 * @details Checks for cap intersections first. For the main body, the normal 
 * points outward, with the Y-component inverted depending on which half 
 * (nappe) of the cone was struck.
 * @param hit          The intersection data.
 * @param object_point The point on the surface in local space.
 * @return             The geometric surface normal.
 */
t_tuple	normal_cone(t_intersection *hit, t_tuple object_point)
{
	double	dist;
	double	y;

	dist = object_point.x * object_point.x + object_point.z * object_point.z;
	if (dist - object_point.y * object_point.y < EPSILON)
	{
		if (object_point.y >= hit->object->data.cone.maximum - EPSILON)
			return (vector(0, 1, 0));
		if (object_point.y <= hit->object->data.cone.minimum + EPSILON)
			return (vector(0, -1, 0));
	}
	y = sqrt(dist);
	if (object_point.y > EPSILON)
		y = -y;
	return (vector(object_point.x, y, object_point.z));
}

/**
 * @fn static void intersect_caps(t_intersections *xs, t_ray *ray, 
 * t_object *object)
 * @brief Checks for intersections against the cone's end caps.
 * @details A cone's radius perfectly mirrors its Y coordinate (absolute 
 * value). Validates cap hits by checking if x^2 + z^2 <= y^2.
 * @param xs     Pointer to the intersections container.
 * @param ray    The transformed local-space ray.
 * @param object The cone being tested.
 */
static void	intersect_caps(t_intersections *xs, t_ray *ray, t_object *object)
{
	double	t;
	double	x;
	double	y;
	double	z;

	if (!object->data.cone.closed || fabs(ray->direction.y) < EPSILON)
		return ;
	y = object->data.cone.minimum;
	t = (y - ray->origin.y) / ray->direction.y;
	x = ray->origin.x + t * ray->direction.x;
	z = ray->origin.z + t * ray->direction.z;
	if (x * x + z * z <= y * y + EPSILON)
		add_intersection(xs, t, object);
	y = object->data.cone.maximum;
	t = (y - ray->origin.y) / ray->direction.y;
	x = ray->origin.x + t * ray->direction.x;
	z = ray->origin.z + t * ray->direction.z;
	if (x * x + z * z <= y * y + EPSILON)
		add_intersection(xs, t, object);
}

/**
 * @fn static void check_truncation(t_intersections *xs, t_ray *ray, 
 * t_object *object, double t)
 * @brief Validates if a calculated hit falls within the cone's height limits.
 * @details Solves for the exact Y coordinate of the intersection point using 
 * the ray equation. If this coordinate falls outside the explicitly defined 
 * minimum and maximum truncation limits of the cone, the mathematical root is 
 * geometrically invalid and safely discarded.
 * @param xs     Pointer to the intersections container.
 * @param ray    The transformed local-space ray.
 * @param object The cone being tested.
 * @param t      The calculated distance to test.
 */
static void	check_truncation(t_intersections *xs, t_ray *ray, t_object *object,
	double t)
{
	double	y;

	y = ray->origin.y + t * ray->direction.y;
	if (object->data.cone.minimum < y && y < object->data.cone.maximum)
		add_intersection(xs, t, object);
}

/**
 * @fn static void calc_cone_coeffs(double *coeffs, t_ray *ray)
 * @brief Derives the quadratic coefficients for a cone.
 * @details Similar to a cylinder, but the Y axis components are subtracted 
 * rather than ignored, creating the double-napped hourglass shape.
 * @param coeffs Array to store the [c, b, a] terms.
 * @param ray    The transformed local-space ray.
 */
static void	calc_cone_coeffs(double *coeffs, t_ray *ray)
{
	coeffs[2] = ray->direction.x * ray->direction.x
		- ray->direction.y * ray->direction.y
		+ ray->direction.z * ray->direction.z;
	coeffs[1] = 2.0 * (ray->origin.x * ray->direction.x
			- ray->origin.y * ray->direction.y
			+ ray->origin.z * ray->direction.z);
	coeffs[0] = ray->origin.x * ray->origin.x
		- ray->origin.y * ray->origin.y
		+ ray->origin.z * ray->origin.z;
}

/**
 * @fn void intersect_cone(t_intersections *xs, t_ray *ray, t_object *object)
 * @brief Calculates ray intersections with a cone.
 * @details Handles the standard quadratic solution, but uniquely manages the 
 * edge case where the ray is parallel to one of the cone's nappes (causing 
 * coeffs[2] to approach 0). In this scenario, it is a single-root linear 
 * equation.
 * @param xs     Pointer to the intersections container.
 * @param ray    The transformed local-space ray.
 * @param object The cone being tested.
 */
void	intersect_cone(t_intersections *xs, t_ray *ray, t_object *object)
{
	double	coeffs[3];
	double	roots[2];
	int		total_roots;
	int		i;

	calc_cone_coeffs(coeffs, ray);
	if (fabs(coeffs[2]) < EPSILON && fabs(coeffs[1]) < EPSILON)
		return ;
	total_roots = solve_quadratic(coeffs, roots);
	if (coeffs[2] < -EPSILON && total_roots == 2)
	{
		check_truncation(xs, ray, object, roots[1]);
		check_truncation(xs, ray, object, roots[0]);
	}
	else
	{
		i = -1;
		while (++i < total_roots)
			check_truncation(xs, ray, object, roots[i]);
	}
	intersect_caps(xs, ray, object);
}
