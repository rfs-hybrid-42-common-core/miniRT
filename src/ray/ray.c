/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 23:01:05 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 01:59:39 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "ray.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn t_ray create_ray(t_tuple origin, t_tuple direction, double time)
 * @brief Initializes a ray and calculates its precomputed optimizations.
 * @details Calculates the `direction_inverse` vectors immediately upon 
 * creation. This allows the AABB slab method to use highly optimized 
 * multiplication and weaponizes IEEE-754 division-by-zero behaviors.
 * @param origin    The starting coordinate.
 * @param direction The normalized direction vector.
 * @param time      The temporal parameter for the ray (motion blur).
 * @return          A fully initialized t_ray structure.
 */
t_ray	create_ray(t_tuple origin, t_tuple direction, double time)
{
	return ((t_ray){
		origin,
		direction,
		vector(1.0 / direction.x, 1.0 / direction.y, 1.0 / direction.z),
		time
	});
}

/**
 * @fn t_tuple ray_position(t_ray *ray, double t)
 * @brief Calculates the exact 3D world coordinate at a given distance `t`.
 * @details Solves the standard ray equation: Position = Origin + Direction * t.
 * @param ray Pointer to the ray.
 * @param t   The distance along the ray.
 * @return    The 3D point in space.
 */
t_tuple	ray_position(t_ray *ray, double t)
{
	return (add_tuples(ray->origin, scaling_tuple(ray->direction, t)));
}

/**
 * @fn void transform_ray(t_ray *out, t_ray *in, t_matrix *matrix)
 * @brief Transforms a ray using a matrix (usually into local object space).
 * @details Multiplies the origin and direction by the given matrix. Also 
 * safely recalculates the `direction_inverse` based on the new direction.
 * @param out    Pointer to the destination ray to populate.
 * @param in     Pointer to the original ray being transformed.
 * @param matrix The transformation matrix to apply.
 */
void	transform_ray(t_ray *out, t_ray *in, t_matrix *matrix)
{
	out->origin = multiply_matrix_tuple(matrix, in->origin);
	out->direction = multiply_matrix_tuple(matrix, in->direction);
	out->direction_inverse = vector(1.0 / out->direction.x,
			1.0 / out->direction.y, 1.0 / out->direction.z);
	out->time = in->time;
}
