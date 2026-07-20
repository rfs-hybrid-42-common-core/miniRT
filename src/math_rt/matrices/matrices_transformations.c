/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrices_transformations.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 21:55:40 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:36:47 by maaugust         ###   ########.fr       */
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
 * @fn t_matrix translation_matrix(double x, double y, double z)
 * @brief Creates a translation matrix to move points in 3D space.
 * @details Modifies the 4th column of an identity matrix. Because vectors have 
 * a w-component of 0.0, multiplying a vector by this matrix will 
 * mathematically ignore the translation, while points (w=1.0) will correctly 
 * shift in 3D space.
 * @param x The X-axis offset.
 * @param y The Y-axis offset.
 * @param z The Z-axis offset.
 * @return  The translation matrix.
 */
t_matrix	translation_matrix(double x, double y, double z)
{
	return ((t_matrix){{
			{1, 0, 0, x},
			{0, 1, 0, y},
			{0, 0, 1, z},
			{0, 0, 0, 1}
		}});
}

/**
 * @fn t_matrix scaling_matrix(double x, double y, double z)
 * @brief Creates a scaling matrix to enlarge or shrink objects.
 * @details Multiplies the x, y, and z components of a tuple by the respective 
 * scalar values on the matrix diagonal. Unlike translation, scaling correctly 
 * affects both points (changing position) and vectors (changing magnitude).
 * @param x The X-axis scale factor.
 * @param y The Y-axis scale factor.
 * @param z The Z-axis scale factor.
 * @return  The scaling matrix.
 */
t_matrix	scaling_matrix(double x, double y, double z)
{
	return ((t_matrix){{
			{x, 0, 0, 0},
			{0, y, 0, 0},
			{0, 0, z, 0},
			{0, 0, 0, 1}
		}});
}

/**
 * @fn t_matrix shearing_matrix(t_shear *shear)
 * @brief Creates a shearing (skew) matrix.
 * @details Skews an object by modifying its coordinate proportionally to its 
 * other coordinates (e.g., 'xy' changes x in proportion to y).
 * @param shear A struct containing the 6 shear proportions.
 * @return      The shearing matrix.
 */
t_matrix	shearing_matrix(t_shear *shear)
{
	return ((t_matrix){{
			{1, shear->xy, shear->xz, 0},
			{shear->yx, 1, shear->yz, 0},
			{shear->zx, shear->zy, 1, 0},
			{0, 0, 0, 1}
		}});
}
