/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrices_inversion.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 21:54:55 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:25:05 by maaugust         ###   ########.fr       */
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
 * @fn t_matrix submatrix(t_matrix *matrix, int del_row, int del_col, int size)
 * @brief Removes a specified row and column from a matrix.
 * @details Used to drop a 4x4 matrix down to a 3x3 (and so on) for determinant 
 * calculations.
 * @param matrix  The original matrix.
 * @param del_row The row index to remove.
 * @param del_col The column index to remove.
 * @param size    The current size of the matrix.
 * @return        The resulting smaller submatrix.
 */
t_matrix	submatrix(t_matrix *matrix, int del_row, int del_col, int size)
{
	t_matrix	sub;
	int			matrix_row;
	int			matrix_col;
	int			sub_row;
	int			sub_col;

	sub = create_matrix();
	sub_row = -1;
	matrix_row = -1;
	while (++matrix_row < size)
	{
		if (matrix_row == del_row)
			continue ;
		sub_row++;
		sub_col = -1;
		matrix_col = -1;
		while (++matrix_col < size)
		{
			if (matrix_col == del_col)
				continue ;
			sub.m[sub_row][++sub_col] = matrix->m[matrix_row][matrix_col];
		}
	}
	return (sub);
}

/**
 * @fn double minor(t_matrix *matrix, int row, int col, int size)
 * @brief Calculates the minor of a matrix cell.
 * @details The minor is the determinant of the submatrix left over after 
 * removing the specified row and column.
 * @param matrix The target matrix.
 * @param row    The row to remove.
 * @param col    The column to remove.
 * @param size   The current size of the matrix.
 * @return       The calculated minor.
 */
double	minor(t_matrix *matrix, int row, int col, int size)
{
	t_matrix	sub;

	sub = submatrix(matrix, row, col, size);
	return (determinant(&sub, size - 1));
}

/**
 * @fn double cofactor(t_matrix *matrix, int row, int col, int size)
 * @brief Calculates the cofactor of a matrix cell.
 * @details The cofactor is the minor, with its sign flipped if the row + col 
 * is an odd number (checkerboard pattern).
 * @param matrix The target matrix.
 * @param row    The row index.
 * @param col    The column index.
 * @param size   The size of the matrix.
 * @return       The calculated cofactor.
 */
double	cofactor(t_matrix *matrix, int row, int col, int size)
{
	if ((row + col) % 2 != 0)
		return (-minor(matrix, row, col, size));
	return (minor(matrix, row, col, size));
}

/**
 * @fn double determinant(t_matrix *matrix, int size)
 * @brief Calculates the determinant of a matrix recursively.
 * @details A determinant of 0 indicates that the matrix is mathematically 
 * uninvertible (singular).
 * @param matrix The matrix to evaluate.
 * @param size   The current size of the matrix.
 * @return       The determinant.
 */
double	determinant(t_matrix *matrix, int size)
{
	double	det;
	double	(*m)[4];
	int		col;

	det = 0;
	m = matrix->m;
	if (size == 2)
		det = m[0][0] * m[1][1] - m[0][1] * m[1][0];
	else
	{
		col = -1;
		while (++col < size)
			det += m[0][col] * cofactor(matrix, 0, col, size);
	}
	return (det);
}

/**
 * @fn t_matrix inverse_matrix(t_matrix *matrix)
 * @brief Inverts a 4x4 matrix.
 * @details The inverse matrix allows rays to be mathematically transformed 
 * into an object's local space for simple, origin-centered intersection math.
 * @param matrix Pointer to the matrix to invert.
 * @return       The inverted matrix. If determinant is 0, returns identity 
 * matrix.
 */
t_matrix	inverse_matrix(t_matrix *matrix)
{
	t_matrix	inverted;
	double		det;
	double		cf;
	int			row;
	int			col;

	det = determinant(matrix, 4);
	if (det == 0)
		return (identity_matrix());
	inverted = create_matrix();
	row = -1;
	while (++row < 4)
	{
		col = -1;
		while (++col < 4)
		{
			cf = cofactor(matrix, row, col, 4);
			inverted.m[col][row] = cf / det;
		}
	}
	return (inverted);
}
