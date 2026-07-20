/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector_operations.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 19:42:39 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:39:14 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "math_rt.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn double dot(t_tuple t1, t_tuple t2)
 * @brief Calculates the dot (scalar) product of two vectors.
 * @details Returns a scalar representing how aligned two vectors are. A dot 
 * product of 1 means they point the same way; 0 means they are perpendicular; 
 * -1 means they point in opposite directions. Crucial for lighting angles.
 * @param t1 The first vector.
 * @param t2 The second vector.
 * @return   The scalar dot product.
 */
double	dot(t_tuple t1, t_tuple t2)
{
	return (t1.x * t2.x + t1.y * t2.y + t1.z * t2.z + t1.w * t2.w);
}

/**
 * @fn t_tuple cross(t_tuple t1, t_tuple t2)
 * @brief Calculates the cross product of two vectors.
 * @details Returns a new vector that is perfectly perpendicular to both input 
 * vectors. Essential for deriving surface normals and TBN matrices.
 * @param t1 The first vector.
 * @param t2 The second vector.
 * @return   The perpendicular cross product vector.
 */
t_tuple	cross(t_tuple t1, t_tuple t2)
{
	return (vector(
			t1.y * t2.z - t1.z * t2.y,
			t1.z * t2.x - t1.x * t2.z,
			t1.x * t2.y - t1.y * t2.x
		)
	);
}

/**
 * @fn double magnitude(t_tuple tuple)
 * @brief Calculates the length (magnitude) of a vector.
 * @details Uses the Pythagorean theorem in 3D space to find the exact distance 
 * a vector spans.
 * @param tuple The vector to measure.
 * @return      The scalar length.
 */
double	magnitude(t_tuple tuple)
{
	return (sqrt(dot(tuple, tuple)));
}

/**
 * @fn t_tuple normalize(t_tuple tuple)
 * @brief Converts a vector into a unit vector.
 * @details Scales the vector so its direction remains identical but its 
 * magnitude becomes exactly 1.0. Essential for accurate ray tracing math.
 * @param tuple The vector to normalize.
 * @return      The normalized unit vector.
 */
t_tuple	normalize(t_tuple tuple)
{
	double	mag;
	double	div;

	mag = magnitude(tuple);
	if (mag < EPSILON)
		return (tuple);
	div = 1.0 / mag;
	return ((t_tuple){
		tuple.x * div,
		tuple.y * div,
		tuple.z * div,
		tuple.w * div
	});
}

/**
 * @fn t_tuple reflect(t_tuple in, t_tuple normal)
 * @brief Bounces an incoming vector off a surface normal.
 * @details Solves the reflection equation. Used for rendering mirrors and 
 * calculating specular highlights.
 * @param in     The incoming vector (pointing towards the surface).
 * @param normal The surface normal vector.
 * @return       The reflected outgoing vector.
 */
t_tuple	reflect(t_tuple in, t_tuple normal)
{
	return (subtract_tuples(in, scaling_tuple(normal, 2.0 * dot(in, normal))));
}
