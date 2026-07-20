/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   torus.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:58:05 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:21:10 by maaugust         ###   ########.fr       */
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
 * @fn t_tuple normal_torus(t_intersection *hit, t_tuple object_point)
 * @brief Calculates the surface normal for a torus.
 * @details Evaluates the algebraic gradient of the torus surface equation 
 * at the specific hit coordinate to find the perpendicular vector.
 * @param hit          The intersection data.
 * @param object_point The point on the surface in local space.
 * @return             The geometric surface normal.
 */
t_tuple	normal_torus(t_intersection *hit, t_tuple object_point)
{
	double	param;
	double	major_r2;

	major_r2 = hit->object->data.torus.major_r
		* hit->object->data.torus.major_r;
	param = (object_point.x * object_point.x)
		+ (object_point.y * object_point.y)
		+ (object_point.z * object_point.z) + major_r2
		- (hit->object->data.torus.minor_r * hit->object->data.torus.minor_r);
	return (normalize(vector(
				object_point.x * param - 2.0 * major_r2 * object_point.x,
				object_point.y * param,
				object_point.z * param - 2.0 * major_r2 * object_point.z)));
}

/**
 * @fn static void calc_torus_coeffs(double *coeffs, t_ray *ray, 
 * t_object *object)
 * @brief Derives the quartic (4th degree) coefficients for a torus.
 * @details The mathematical expansion of intersecting a line with a torus 
 * results in a large algebraic polynomial. This isolates the terms to pass 
 * into the quartic root solver.
 * @param coeffs Array to store the [e, d, c, b, a] quartic coefficients.
 * @param ray    The transformed local-space ray.
 * @param object The torus being evaluated.
 */
static void	calc_torus_coeffs(double *coeffs, t_ray *ray, t_object *object)
{
	double	major_r2;
	double	v[5];

	major_r2 = object->data.torus.major_r * object->data.torus.major_r;
	v[0] = (ray->origin.x * ray->origin.x)
		+ (ray->origin.y * ray->origin.y)
		+ (ray->origin.z * ray->origin.z)
		+ major_r2 - (object->data.torus.minor_r * object->data.torus.minor_r);
	v[1] = dot(ray->origin, ray->direction);
	v[2] = (ray->direction.x * ray->direction.x)
		+ (ray->direction.z * ray->direction.z);
	v[3] = (ray->origin.x * ray->direction.x)
		+ (ray->origin.z * ray->direction.z);
	v[4] = (ray->origin.x * ray->origin.x)
		+ (ray->origin.z * ray->origin.z);
	coeffs[4] = 1.0;
	coeffs[3] = 4.0 * v[1];
	coeffs[2] = 2.0 * v[0] + 4.0 * (v[1] * v[1]) - 4.0 * major_r2 * v[2];
	coeffs[1] = 4.0 * v[0] * v[1] - 8.0 * major_r2 * v[3];
	coeffs[0] = (v[0] * v[0]) - 4.0 * major_r2 * v[4];
}

/**
 * @fn void intersect_torus(t_intersections *xs, t_ray *ray, t_object *object)
 * @brief Calculates ray intersections with a torus.
 * @details Because a torus has a "hole", a single ray can enter and exit the 
 * geometry twice, resulting in up to 4 real intersection roots.
 * @param xs     Pointer to the intersections container.
 * @param ray    The transformed local-space ray.
 * @param object The torus being tested.
 */
void	intersect_torus(t_intersections *xs, t_ray *ray, t_object *object)
{
	double	coeffs[5];
	double	roots[4];
	int		total_roots;
	int		i;

	calc_torus_coeffs(coeffs, ray, object);
	total_roots = solve_quartic(coeffs, roots);
	i = -1;
	while (++i < total_roots)
		add_intersection(xs, roots[i], object);
}
