/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrices_core.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 22:08:47 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:35:15 by maaugust         ###   ########.fr       */
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
 * @fn t_matrix create_matrix(void)
 * @brief Initializes a 4x4 matrix initialized to all zeros.
 * @details Used as a clean slate before populating specific elements (like 
 * submatrices or inverse matrices), ensuring no stack garbage memory values 
 * interfere with the linear algebra operations.
 * @return A blank t_matrix struct.
 */
t_matrix	create_matrix(void)
{
	return ((t_matrix){{{0}}});
}

/**
 * @fn t_matrix identity_matrix(void)
 * @brief Creates a 4x4 Identity Matrix.
 * @details An identity matrix acts like the number '1' in matrix 
 * multiplication. Multiplying a tuple by this matrix leaves the tuple 
 * unchanged.
 * @return The Identity Matrix.
 */
t_matrix	identity_matrix(void)
{
	return ((t_matrix){{
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1}
		}});
}

/**
 * @fn bool equal_matrices(t_matrix *m1, t_matrix *m2)
 * @brief Compares two 4x4 matrices for equality.
 * @details Uses the defined EPSILON to account for floating-point inaccuracies.
 * @param m1 Pointer to the first matrix.
 * @param m2 Pointer to the second matrix.
 * @return   True if all elements are within EPSILON, false otherwise.
 */
bool	equal_matrices(t_matrix *m1, t_matrix *m2)
{
	int	row;
	int	col;

	row = -1;
	while (++row < 4)
	{
		col = -1;
		while (++col < 4)
			if (fabs(m1->m[row][col] - m2->m[row][col]) > EPSILON)
				return (false);
	}
	return (true);
}
