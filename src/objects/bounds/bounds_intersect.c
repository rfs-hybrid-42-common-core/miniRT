/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bounds_intersect.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 02:47:35 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 03:03:55 by maaugust         ###   ########.fr       */
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
 * @fn static void check_axis(double origin, double direction_inverse, 
 * double *limits, double *t_vals)
 * @brief Calculates the intersection distance for a single 1D axis slice.
 * @details Multiplies by direction_inverse to avoid costly division.
 * @param origin            The ray's origin on this axis.
 * @param direction_inverse The inverted ray direction on this axis.
 * @param limits            The [min, max] bounds on this axis.
 * @param t_vals            Array to store the calculated [t_enter, t_exit].
 */
static void	check_axis(double origin, double direction_inverse, double *limits,
	double *t_vals)
{
	double	tmin;
	double	tmax;
	double	tmp;

	tmin = (limits[0] - origin) * direction_inverse;
	tmax = (limits[1] - origin) * direction_inverse;
	if (tmin > tmax)
	{
		tmp = tmin;
		tmin = tmax;
		tmax = tmp;
	}
	t_vals[0] = tmin;
	t_vals[1] = tmax;
}

/**
 * @fn bool intersect_bounds(t_ray *ray, t_bounds *bounds)
 * @brief Checks if a ray intersects an Axis-Aligned Bounding Box (AABB).
 * @details Implements the "Slab Method". Tests X, Y, and Z axes sequentially 
 * and short-circuits early if the ray clearly misses the volume. Crucial for 
 * fast BVH traversal.
 * @param ray    The ray to test.
 * @param bounds The bounding box to test against.
 * @return       True if the ray passes through the box, false otherwise.
 */
bool	intersect_bounds(t_ray *ray, t_bounds *bounds)
{
	double	t_vals[2];
	double	t_enter;
	double	t_exit;
	double	limits[2];

	limits[0] = bounds->min.x;
	limits[1] = bounds->max.x;
	check_axis(ray->origin.x, ray->direction_inverse.x, limits, t_vals);
	t_enter = t_vals[0];
	t_exit = t_vals[1];
	limits[0] = bounds->min.y;
	limits[1] = bounds->max.y;
	check_axis(ray->origin.y, ray->direction_inverse.y, limits, t_vals);
	t_enter = get_fmax(t_enter, t_vals[0]);
	t_exit = get_fmin(t_exit, t_vals[1]);
	if (t_enter > t_exit + EPSILON)
		return (false);
	limits[0] = bounds->min.z;
	limits[1] = bounds->max.z;
	check_axis(ray->origin.z, ray->direction_inverse.z, limits, t_vals);
	t_enter = get_fmax(t_enter, t_vals[0]);
	t_exit = get_fmin(t_exit, t_vals[1]);
	if (t_enter > t_exit + EPSILON)
		return (false);
	return (true);
}
