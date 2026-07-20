/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 00:21:26 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:14:33 by maaugust         ###   ########.fr       */
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
 * @fn t_tuple normal_sphere(t_intersection *hit, t_tuple object_point)
 * @brief Calculates the surface normal for a sphere.
 * @details Since the sphere is evaluated in local space at the origin (0,0,0), 
 * the normal is simply the vector pointing from the origin to the hit point.
 * @param hit          The intersection data (unused for standard spheres).
 * @param object_point The point on the surface in local space.
 * @return             The unnormalized geometric surface normal.
 */
t_tuple	normal_sphere(t_intersection *hit, t_tuple object_point)
{
	(void)hit;
	return (subtract_tuples(object_point, point(0, 0, 0)));
}

/**
 * @fn void intersect_sphere(t_intersections *xs, t_ray *ray, t_object *object)
 * @brief Calculates ray intersections with a sphere.
 * @details Solves the quadratic equation representing the intersection of a 
 * line and a unit sphere. If roots are found, they are added to the list.
 * @param xs     Pointer to the intersections container.
 * @param ray    The transformed local-space ray.
 * @param object The sphere being tested.
 */
void	intersect_sphere(t_intersections *xs, t_ray *ray, t_object *object)
{
	t_tuple	sp_to_ray;
	double	coeffs[3];
	double	roots[2];
	int		total_roots;
	int		i;

	sp_to_ray = subtract_tuples(ray->origin, object->data.sphere.origin);
	coeffs[2] = dot(ray->direction, ray->direction);
	coeffs[1] = 2.0 * dot(ray->direction, sp_to_ray);
	coeffs[0] = dot(sp_to_ray, sp_to_ray) - 1.0;
	total_roots = solve_quadratic(coeffs, roots);
	i = -1;
	while (++i < total_roots)
		add_intersection(xs, roots[i], object);
}
