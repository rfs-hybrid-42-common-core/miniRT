/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   view_transform.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 03:12:22 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/05 21:32:41 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "camera.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static t_matrix get_orientation(t_tuple left, t_tuple true_up, 
 * t_tuple forward)
 * @brief Constructs the orientation portion of the view matrix.
 * @details Maps the camera's local left, up, and forward vectors directly into 
 * the rows of a 4x4 matrix. This aligns the camera's local coordinate system 
 * with the world's coordinate system.
 * @param left    The normalized vector pointing to the camera's local left.
 * @param true_up The normalized vector pointing to the camera's local up.
 * @param forward The normalized vector pointing in the direction of the view.
 * @return        A 4x4 matrix representing pure rotation/orientation.
 */
static t_matrix	get_orientation(t_tuple left, t_tuple true_up, t_tuple forward)
{
	return ((t_matrix){{
			{left.x, left.y, left.z, 0},
			{true_up.x, true_up.y, true_up.z, 0},
			{-forward.x, -forward.y, -forward.z, 0},
			{0, 0, 0, 1}
		}});
}

/**
 * @fn t_matrix view_transform(t_tuple from, t_tuple to, t_tuple up)
 * @brief Constructs a view transformation matrix for the camera.
 * @details Computes the forward, left, and true_up vectors to create an 
 * orientation matrix, which is combined with a translation matrix to map 
 * world space into camera space. Automatically handles collinearity when 
 * looking directly up by adjusting the reference up vector to maintain 
 * spatial orientation.
 * @param from The absolute position of the camera (the "eye").
 * @param to   The point in 3D space the camera is looking at.
 * @param up   A vector dictating which way is "up" (usually 0, 1, 0).
 * @return     A complete view transformation matrix.
 */
t_matrix	view_transform(t_tuple from, t_tuple to, t_tuple up)
{
	t_tuple		forward;
	t_tuple		left;
	t_tuple		true_up;
	t_matrix	orientation;
	t_matrix	translated;

	forward = normalize(subtract_tuples(to, from));
	if (forward.y > 1.0 - EPSILON)
		up = vector(0, 0, -1);
	left = normalize(cross(forward, normalize(up)));
	true_up = normalize(cross(left, forward));
	orientation = get_orientation(left, true_up, forward);
	translated = translation_matrix(-from.x, -from.y, -from.z);
	return (multiply_matrices(&orientation, &translated));
}
