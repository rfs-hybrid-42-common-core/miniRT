/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrices_quats.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 14:36:24 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/10 17:46:20 by maaugust         ###   ########.fr       */
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
 * @fn static t_quat extract_negative_trace(t_matrix *matrix)
 * @brief Handles quaternion extraction for matrices with a non-positive trace.
 * @details Numerically stable fallback for matrix_to_quat when the sum of the 
 * diagonal elements is less than zero, preventing division by zero or 
 * imaginary scaling artifacts.
 * @param matrix The pure rotation matrix to evaluate.
 * @return       The extracted quaternion.
 */
static t_quat	extract_negative_trace(t_matrix *matrix)
{
	double	scale;

	if (matrix->m[0][0] > matrix->m[1][1] && matrix->m[0][0] > matrix->m[2][2])
	{
		scale = 2.0 * sqrt(1.0 + matrix->m[0][0] - matrix->m[1][1]
				- matrix->m[2][2]);
		return ((t_quat){(matrix->m[2][1] - matrix->m[1][2]) / scale,
			0.25 * scale, (matrix->m[0][1] + matrix->m[1][0]) / scale,
			(matrix->m[0][2] + matrix->m[2][0]) / scale});
	}
	else if (matrix->m[1][1] > matrix->m[2][2])
	{
		scale = 2.0 * sqrt(1.0 + matrix->m[1][1] - matrix->m[0][0]
				- matrix->m[2][2]);
		return ((t_quat){(matrix->m[0][2] - matrix->m[2][0]) / scale,
		(matrix->m[0][1] + matrix->m[1][0]) / scale,
		0.25 * scale, (matrix->m[1][2] + matrix->m[2][1]) / scale});
	}
	scale = 2.0 * sqrt(1.0 + matrix->m[2][2] - matrix->m[0][0]
			- matrix->m[1][1]);
	return ((t_quat){(matrix->m[1][0] - matrix->m[0][1]) / scale,
		(matrix->m[0][2] + matrix->m[2][0]) / scale,
		(matrix->m[1][2] + matrix->m[2][1]) / scale, 0.25 * scale});
}

/**
 * @fn t_quat matrix_to_quat(t_matrix *matrix)
 * @brief Converts a pure 3x3 rotation matrix into a Quaternion.
 * @details Required step to mathematically prepare two different rotations 
 * for Spherical Linear Interpolation (SLERP).
 * @param matrix Pointer to the pure rotation matrix.
 * @return       The computed Quaternion.
 */
t_quat	matrix_to_quat(t_matrix *matrix)
{
	double	trace;
	double	scale;

	trace = matrix->m[0][0] + matrix->m[1][1] + matrix->m[2][2];
	if (trace > 0.0)
	{
		scale = 0.5 / sqrt(trace + 1.0);
		return ((t_quat){0.25 / scale,
			(matrix->m[2][1] - matrix->m[1][2]) * scale,
			(matrix->m[0][2] - matrix->m[2][0]) * scale,
			(matrix->m[1][0] - matrix->m[0][1]) * scale});
	}
	return (extract_negative_trace(matrix));
}

/**
 * @fn t_matrix quat_to_matrix(t_quat *quat)
 * @brief Converts a Quaternion back into a 4x4 rotation matrix.
 * @details Re-establishes standard matrix mathematics after SLERP is completed.
 * @param quat Pointer to the Quaternion.
 * @return     A 4x4 pure rotation matrix.
 */
t_matrix	quat_to_matrix(t_quat *quat)
{
	t_matrix	matrix;

	matrix = identity_matrix();
	matrix.m[0][0] = 1.0 - 2.0 * (quat->y * quat->y + quat->z * quat->z);
	matrix.m[0][1] = 2.0 * (quat->x * quat->y - quat->w * quat->z);
	matrix.m[0][2] = 2.0 * (quat->x * quat->z + quat->w * quat->y);
	matrix.m[1][0] = 2.0 * (quat->x * quat->y + quat->w * quat->z);
	matrix.m[1][1] = 1.0 - 2.0 * (quat->x * quat->x + quat->z * quat->z);
	matrix.m[1][2] = 2.0 * (quat->y * quat->z - quat->w * quat->x);
	matrix.m[2][0] = 2.0 * (quat->x * quat->z - quat->w * quat->y);
	matrix.m[2][1] = 2.0 * (quat->y * quat->z + quat->w * quat->x);
	matrix.m[2][2] = 1.0 - 2.0 * (quat->x * quat->x + quat->y * quat->y);
	return (matrix);
}

/**
 * @fn static t_quat normalize_quat(t_quat quat)
 * @brief Normalizes a Quaternion.
 * @details Forces the quaternion's magnitude to 1.0, ensuring it represents a 
 * valid spatial rotation without accidentally introducing uniform scale.
 * @param quat The Quaternion to normalize.
 * @return     The normalized Quaternion.
 */
static t_quat	normalize_quat(t_quat quat)
{
	double	mag;
	double	div;

	mag = sqrt(quat.w * quat.w + quat.x * quat.x
			+ quat.y * quat.y + quat.z * quat.z);
	if (mag < EPSILON)
		return (quat);
	div = 1.0 / mag;
	return ((t_quat){
		quat.w * div,
		quat.x * div,
		quat.y * div,
		quat.z * div
	});
}

/**
 * @fn t_quat slerp_quat(t_quat q1, t_quat q2, double t)
 * @brief Performs Spherical Linear Interpolation between two Quaternions.
 * @details Finds the shortest rotational path between two orientations, 
 * allowing for physically accurate motion blur without gimbal lock or spatial 
 * warping.
 * @param q1 The starting Quaternion.
 * @param q2 The ending Quaternion.
 * @param t  The interpolation progress (0.0 to 1.0).
 * @return   The interpolated Quaternion.
 */
t_quat	slerp_quat(t_quat q1, t_quat q2, double t)
{
	t_quat	quat;
	double	cos_o;
	double	sin_o;
	double	omega;
	double	k[2];

	cos_o = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
	if (cos_o < 0.0)
	{
		cos_o = -cos_o;
		q2 = (t_quat){-q2.w, -q2.x, -q2.y, -q2.z};
	}
	if (cos_o > 0.9995)
	{
		quat = (t_quat){q1.w + t * (q2.w - q1.w), q1.x + t * (q2.x - q1.x),
			q1.y + t * (q2.y - q1.y), q1.z + t * (q2.z - q1.z)};
		return (normalize_quat(quat));
	}
	omega = acos(cos_o);
	sin_o = sin(omega);
	k[0] = sin((1.0 - t) * omega) / sin_o;
	k[1] = sin(t * omega) / sin_o;
	quat = (t_quat){q1.w * k[0] + q2.w * k[1], q1.x * k[0] + q2.x * k[1],
		q1.y * k[0] + q2.y * k[1], q1.z * k[0] + q2.z * k[1]};
	return (quat);
}
