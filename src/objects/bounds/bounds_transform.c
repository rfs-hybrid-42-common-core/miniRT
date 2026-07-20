/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bounds_transform.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 15:27:59 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 14:06:58 by maaugust         ###   ########.fr       */
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
 * @fn static void set_vals(double *min, double *max, t_bounds *box, int axis)
 * @brief Extracts the min and max values from a bounding box for a specific 
 * axis.
 * @details Helper function used to isolate the 1D spatial extents during the 
 * highly optimized Jim Arvo AABB transformation algorithm.
 * @param min  Pointer to store the minimum value.
 * @param max  Pointer to store the maximum value.
 * @param box  The target bounding box.
 * @param axis The requested axis (0=X, 1=Y, 2=Z).
 */
static void	set_vals(double *min, double *max, t_bounds *box, int axis)
{
	if (axis == 0)
	{
		*min = box->min.x;
		*max = box->max.x;
	}
	else if (axis == 1)
	{
		*min = box->min.y;
		*max = box->max.y;
	}
	else
	{
		*min = box->min.z;
		*max = box->max.z;
	}
}

/**
 * @fn static void extract_axis(t_bounds *res, t_bounds *box, 
 * t_matrix *transform, * int axis)
 * @brief Transforms a single axis of an AABB.
 * @details Implements a core piece of Jim Arvo's algorithm to multiply minimum 
 * and maximum extents directly by the matrix tensor, avoiding the need to 
 * fully transform and re-evaluate all 8 corners of the box.
 * @param res       The resulting transformed box being built.
 * @param box       The original local-space box.
 * @param transform The transformation matrix.
 * @param axis      The current axis being calculated.
 */
static void	extract_axis(t_bounds *res, t_bounds *box, t_matrix *transform,
	int axis)
{
	double	(*m)[4];
	double	min;
	double	max;

	m = transform->m;
	set_vals(&min, &max, box, axis);
	res->min.x += get_fmin(m[0][axis] * min, m[0][axis] * max);
	res->max.x += get_fmax(m[0][axis] * min, m[0][axis] * max);
	res->min.y += get_fmin(m[1][axis] * min, m[1][axis] * max);
	res->max.y += get_fmax(m[1][axis] * min, m[1][axis] * max);
	res->min.z += get_fmin(m[2][axis] * min, m[2][axis] * max);
	res->max.z += get_fmax(m[2][axis] * min, m[2][axis] * max);
}

/**
 * @fn t_bounds transform_bounds(t_bounds *box, t_matrix *transform)
 * @brief Transforms an AABB using Jim Arvo's highly optimized algorithm.
 * @details Massively speeds up BVH creation by extracting the matrix 
 * translation immediately, then adding the scaled extents axis by axis.
 * @param box       The original local-space bounding box.
 * @param transform The transformation matrix to apply.
 * @return          The accurately scaled and translated world-space box.
 */
t_bounds	transform_bounds(t_bounds *box, t_matrix *transform)
{
	t_bounds	res;
	double		(*m)[4];

	m = transform->m;
	res.min = point(m[0][3], m[1][3], m[2][3]);
	res.max = point(m[0][3], m[1][3], m[2][3]);
	extract_axis(&res, box, transform, 0);
	extract_axis(&res, box, transform, 1);
	extract_axis(&res, box, transform, 2);
	return (res);
}
