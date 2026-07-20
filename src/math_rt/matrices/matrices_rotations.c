/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrices_rotations.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 18:52:51 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/03 15:50:29 by maaugust         ###   ########.fr       */
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
 * @fn t_matrix rotation_matrix_x(double rad)
 * @brief Creates a rotation matrix around the X axis.
 * @details Uses the trigonometric sine and cosine of the given angle to 
 * construct a matrix that rotates a point or vector around the X axis.
 * @param rad Rotation angle in radians.
 * @return    The X-axis rotation matrix.
 */
t_matrix	rotation_matrix_x(double rad)
{
	double	c;
	double	s;

	c = cos(rad);
	s = sin(rad);
	return ((t_matrix){{
			{1, 0, 0, 0},
			{0, c, -s, 0},
			{0, s, c, 0},
			{0, 0, 0, 1}
		}});
}

/**
 * @fn t_matrix rotation_matrix_y(double rad)
 * @brief Creates a rotation matrix around the Y axis.
 * @details Evaluates the sine and cosine to construct the Y-axis rotation 
 * matrix. Notice the negative sine is on the bottom-left instead of the 
 * top-right to maintain a consistent Left-Handed coordinate system rotation.
 * @param rad Rotation angle in radians.
 * @return    The Y-axis rotation matrix.
 */
t_matrix	rotation_matrix_y(double rad)
{
	double	c;
	double	s;

	c = cos(rad);
	s = sin(rad);
	return ((t_matrix){{
			{c, 0, s, 0},
			{0, 1, 0, 0},
			{-s, 0, c, 0},
			{0, 0, 0, 1}
		}});
}

/**
 * @fn t_matrix rotation_matrix_z(double rad)
 * @brief Creates a rotation matrix around the Z axis.
 * @details Constructs a matrix that rotates coordinates along the XY plane, 
 * treating the Z axis as the pivot.
 * @param rad Rotation angle in radians.
 * @return    The Z-axis rotation matrix.
 */
t_matrix	rotation_matrix_z(double rad)
{
	double	c;
	double	s;

	c = cos(rad);
	s = sin(rad);
	return ((t_matrix){{
			{c, -s, 0, 0},
			{s, c, 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1}
		}});
}

/**
 * @fn t_matrix combined_rotation_matrix(t_tuple rotation)
 * @brief Combines X, Y, and Z Euler rotations into a single matrix.
 * @details Multiplies the individual rotation matrices in the order of 
 * Z * Y * X to produce a unified transformation matrix. This allows objects 
 * to undergo complex pitch, yaw, and roll transformations simultaneously.
 * @param rotation A tuple containing the X, Y, and Z rotation angles in 
 * radians.
 * @return         The final aggregated rotation matrix.
 */
t_matrix	combined_rotation_matrix(t_tuple rotation)
{
	t_matrix	rx;
	t_matrix	ry;
	t_matrix	rz;
	t_matrix	tmp;

	rx = rotation_matrix_x(rotation.x);
	ry = rotation_matrix_y(rotation.y);
	rz = rotation_matrix_z(rotation.z);
	tmp = multiply_matrices(&ry, &rx);
	return (multiply_matrices(&rz, &tmp));
}
