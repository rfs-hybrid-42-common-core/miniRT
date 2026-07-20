/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrices_operations.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 21:54:46 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:26:21 by maaugust         ###   ########.fr       */
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
 * @fn t_matrix transpose_matrix(t_matrix *matrix)
 * @brief Transposes a 4x4 matrix.
 * @details Converts rows into columns and columns into rows. Used to properly 
 * transform normals to avoid skewing when an object undergoes non-uniform 
 * scaling.
 * @param matrix Pointer to the matrix to transpose.
 * @return       The transposed matrix.
 */
t_matrix	transpose_matrix(t_matrix *matrix)
{
	t_matrix	transposed;
	int			row;
	int			col;

	transposed = create_matrix();
	row = -1;
	while (++row < 4)
	{
		col = -1;
		while (++col < 4)
			transposed.m[row][col] = matrix->m[col][row];
	}
	return (transposed);
}

/**
 * @fn t_matrix multiply_matrices(t_matrix *m1, t_matrix *m2)
 * @brief Multiplies two 4x4 matrices together.
 * @details Combines two distinct transformations into a single matrix.
 * @param m1 Pointer to the left matrix.
 * @param m2 Pointer to the right matrix.
 * @return   The multiplied composite matrix.
 */
t_matrix	multiply_matrices(t_matrix *m1, t_matrix *m2)
{
	t_matrix	multiplied;
	int			row;
	int			col;

	multiplied = create_matrix();
	row = -1;
	while (++row < 4)
	{
		col = -1;
		while (++col < 4)
			multiplied.m[row][col] = m1->m[row][0] * m2->m[0][col]
				+ m1->m[row][1] * m2->m[1][col]
				+ m1->m[row][2] * m2->m[2][col]
				+ m1->m[row][3] * m2->m[3][col];
	}
	return (multiplied);
}

/**
 * @fn t_tuple multiply_matrix_tuple(t_matrix *matrix, t_tuple tuple)
 * @brief Multiplies a tuple by a matrix.
 * @details Applies the matrix's spatial transformation to a point or vector.
 * @param matrix Pointer to the transformation matrix.
 * @param tuple  The point or vector to transform.
 * @return       The transformed tuple.
 */
t_tuple	multiply_matrix_tuple(t_matrix *matrix, t_tuple tuple)
{
	double	(*m)[4];

	m = matrix->m;
	return ((t_tuple){
		m[0][0] * tuple.x + m[0][1] * tuple.y
		+ m[0][2] * tuple.z + m[0][3] * tuple.w,
		m[1][0] * tuple.x + m[1][1] * tuple.y
		+ m[1][2] * tuple.z + m[1][3] * tuple.w,
		m[2][0] * tuple.x + m[2][1] * tuple.y
		+ m[2][2] * tuple.z + m[2][3] * tuple.w,
		m[3][0] * tuple.x + m[3][1] * tuple.y
		+ m[3][2] * tuple.z + m[3][3] * tuple.w
	});
}

/**
 * @fn t_matrix align_normal_matrix(t_tuple normal)
 * @brief Creates a rotation matrix that aligns an object to a target normal.
 * @details Uses the Gram-Schmidt orthogonalization process. Safely handles the 
 * cross-product singularity when the target normal points straight up or down.
 * @param normal The normalized 3D orientation vector to align to.
 * @return       The rotation matrix.
 */
t_matrix	align_normal_matrix(t_tuple normal)
{
	t_matrix	matrix;
	t_tuple		right;
	t_tuple		forward;

	matrix = identity_matrix();
	if (fabs(normal.y - 1.0) < EPSILON)
		return (matrix);
	if (fabs(normal.y + 1.0) < EPSILON)
	{
		matrix.m[0][0] = -1.0;
		matrix.m[1][1] = -1.0;
		return (matrix);
	}
	right = normalize(cross(vector(0, 1, 0), normal));
	forward = cross(normal, right);
	matrix.m[0][0] = right.x;
	matrix.m[1][0] = right.y;
	matrix.m[2][0] = right.z;
	matrix.m[0][1] = normal.x;
	matrix.m[1][1] = normal.y;
	matrix.m[2][1] = normal.z;
	matrix.m[0][2] = forward.x;
	matrix.m[1][2] = forward.y;
	matrix.m[2][2] = forward.z;
	return (matrix);
}
