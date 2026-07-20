/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrices_interpolation.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 18:52:28 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:22:27 by maaugust         ###   ########.fr       */
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
 * @fn static void extract_comps(t_matrix *matrix, t_tuple *translation, 
 * t_tuple *scale, t_matrix *rotation)
 * @brief Decomposes a transformation matrix into TRS components.
 * @details Extracts the Translation vector, Scale vector, and pure Rotation 
 * matrix. Essential for accurately interpolating motion blur.
 * @param matrix      The composite transform matrix to decompose.
 * @param translation Pointer to store the extracted translation point.
 * @param scale       Pointer to store the extracted scale vector.
 * @param rotation    Pointer to store the extracted pure rotation matrix.
 */
static void	extract_comps(t_matrix *matrix, t_tuple *translation,
	t_tuple *scale, t_matrix *rotation)
{
	int	i;

	*translation = point(matrix->m[0][3], matrix->m[1][3], matrix->m[2][3]);
	*scale = vector(
			sqrt(matrix->m[0][0] * matrix->m[0][0]
				+ matrix->m[1][0] * matrix->m[1][0]
				+ matrix->m[2][0] * matrix->m[2][0]),
			sqrt(matrix->m[0][1] * matrix->m[0][1]
				+ matrix->m[1][1] * matrix->m[1][1]
				+ matrix->m[2][1] * matrix->m[2][1]),
			sqrt(matrix->m[0][2] * matrix->m[0][2]
				+ matrix->m[1][2] * matrix->m[1][2]
				+ matrix->m[2][2] * matrix->m[2][2]));
	*rotation = identity_matrix();
	i = -1;
	while (++i < 3)
	{
		rotation->m[i][0] = matrix->m[i][0] / scale->x;
		rotation->m[i][1] = matrix->m[i][1] / scale->y;
		rotation->m[i][2] = matrix->m[i][2] / scale->z;
	}
}

/**
 * @fn t_matrix interpolate_transform(t_matrix *start, t_matrix *end, double t)
 * @brief Interpolates smoothly between two transformation matrices.
 * @details Used for temporal motion blur. Decomposes the matrices, performs a 
 * Linear Interpolation (LERP) on the translation/scale, and a Spherical Linear 
 * Interpolation (SLERP) on the rotation via Quaternions to prevent warping.
 * @param start The initial transformation matrix.
 * @param end   The final transformation matrix.
 * @param t     The interpolation factor (0.0 to 1.0).
 * @return      The correctly interpolated composite matrix.
 */
t_matrix	interpolate_transform(t_matrix *start, t_matrix *end, double t)
{
	t_tuple		translation[3];
	t_tuple		scale[3];
	t_matrix	rotation[3];
	t_quat		quat[3];
	t_matrix	matrix[2];

	extract_comps(start, &translation[0], &scale[0], &rotation[0]);
	extract_comps(end, &translation[1], &scale[1], &rotation[1]);
	translation[2] = add_tuples(translation[0], scaling_tuple(
				subtract_tuples(translation[1], translation[0]), t));
	scale[2] = add_tuples(scale[0], scaling_tuple(
				subtract_tuples(scale[1], scale[0]), t));
	quat[0] = matrix_to_quat(&rotation[0]);
	quat[1] = matrix_to_quat(&rotation[1]);
	quat[2] = slerp_quat(quat[0], quat[1], t);
	rotation[2] = quat_to_matrix(&quat[2]);
	matrix[0] = scaling_matrix(scale[2].x, scale[2].y, scale[2].z);
	matrix[1] = translation_matrix(translation[2].x, translation[2].y,
			translation[2].z);
	rotation[0] = multiply_matrices(&rotation[2], &matrix[0]);
	return (multiply_matrices(&matrix[1], &rotation[0]));
}
