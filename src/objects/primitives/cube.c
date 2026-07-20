/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cube.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 18:14:03 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:17:11 by maaugust         ###   ########.fr       */
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
 * @fn t_tuple normal_cube(t_intersection *hit, t_tuple object_point)
 * @brief Calculates the surface normal for a cube.
 * @details Determines which of the 6 faces was hit by finding the component 
 * (X, Y, or Z) with the largest absolute value (closest to 1.0 or -1.0).
 * @param hit          The intersection data (unused).
 * @param object_point The point on the surface in local space.
 * @return             The normal perpendicular to the hit face.
 */
t_tuple	normal_cube(t_intersection *hit, t_tuple object_point)
{
	double	abs[3];
	double	maxc;

	(void)hit;
	abs[0] = fabs(object_point.x);
	abs[1] = fabs(object_point.y);
	abs[2] = fabs(object_point.z);
	maxc = get_fmax(abs[0], get_fmax(abs[1], abs[2]));
	if (maxc == abs[0])
		return (vector(object_point.x, 0, 0));
	if (maxc == abs[1])
		return (vector(0, object_point.y, 0));
	return (vector(0, 0, object_point.z));
}

/**
 * @fn static void check_axis(double origin, double direction_inverse, 
 * double *t_vals)
 * @brief Evaluates ray intersection limits for a single axis of the cube.
 * @details Uses the Slab Method (multiplying by direction_inverse) to find 
 * where the ray enters and exits the space between -1.0 and 1.0 on this axis.
 * @param origin            The ray's origin on this axis.
 * @param direction_inverse The inverted ray direction on this axis.
 * @param t_vals            Array to store the calculated [t_enter, t_exit].
 */
static void	check_axis(double origin, double direction_inverse, double *t_vals)
{
	double	tmin;
	double	tmax;
	double	tmp;

	tmin = (-1.0 - origin) * direction_inverse;
	tmax = (1.0 - origin) * direction_inverse;
	if (tmin > tmax + EPSILON)
	{
		tmp = tmin;
		tmin = tmax;
		tmax = tmp;
	}
	t_vals[0] = tmin;
	t_vals[1] = tmax;
}

/**
 * @fn void intersect_cube(t_intersections *xs, t_ray *ray, t_object *object)
 * @brief Calculates ray intersections with a unit cube.
 * @details Uses the Slab Method to overlap the enter/exit distances across 
 * the X, Y, and Z axes. If the overlap is mathematically valid, the ray has 
 * struck the cube.
 * @param xs     Pointer to the intersections container.
 * @param ray    The transformed local-space ray.
 * @param object The cube being tested.
 */
void	intersect_cube(t_intersections *xs, t_ray *ray, t_object *object)
{
	double	t_vals[2];
	double	t_enter;
	double	t_exit;

	check_axis(ray->origin.x, ray->direction_inverse.x, t_vals);
	t_enter = t_vals[0];
	t_exit = t_vals[1];
	check_axis(ray->origin.y, ray->direction_inverse.y, t_vals);
	t_enter = get_fmax(t_enter, t_vals[0]);
	t_exit = get_fmin(t_exit, t_vals[1]);
	if (t_enter > t_exit + EPSILON)
		return ;
	check_axis(ray->origin.z, ray->direction_inverse.z, t_vals);
	t_enter = get_fmax(t_enter, t_vals[0]);
	t_exit = get_fmin(t_exit, t_vals[1]);
	if (t_enter > t_exit + EPSILON)
		return ;
	add_intersection(xs, t_enter, object);
	add_intersection(xs, t_exit, object);
}
