/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uv_cube.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 02:22:57 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:55:42 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "patterns.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static t_cube_face get_cube_face(t_tuple point)
 * @brief Identifies which of the 6 cube faces the point resides on.
 * @details Evaluates the highest absolute spatial coordinate.
 * @param point The local coordinate.
 * @return      The specific cube face enumeration.
 */
static t_cube_face	get_cube_face(t_tuple point)
{
	double	coord;

	coord = get_fmax(fabs(point.x), get_fmax(fabs(point.y), fabs(point.z)));
	if (coord == point.x)
		return (FACE_RIGHT);
	if (coord == -point.x)
		return (FACE_LEFT);
	if (coord == point.y)
		return (FACE_UP);
	if (coord == -point.y)
		return (FACE_DOWN);
	if (coord == point.z)
		return (FACE_FRONT);
	return (FACE_BACK);
}

/**
 * @fn static void map_left_right(t_tuple point, t_uv_coord *uv_coord)
 * @brief Processes 2D wrapping for the X-axis faces.
 * @details Maps the 3D local coordinates of the +X and -X faces of a cube down 
 * to 2D U/V space, adjusting bounds to fit strictly within the [0.0, 1.0] 
 * scale.
 * @param point    The local coordinate.
 * @param uv_coord Pointer to the uv data struct.
 */
static void	map_left_right(t_tuple point, t_uv_coord *uv_coord)
{
	if (uv_coord->cube_face == FACE_RIGHT)
	{
		uv_coord->u = fmod(1.0 - point.z, 2.0) / 2.0;
		uv_coord->v = fmod(point.y + 1.0, 2.0) / 2.0;
	}
	else
	{
		uv_coord->u = fmod(point.z + 1.0, 2.0) / 2.0;
		uv_coord->v = fmod(point.y + 1.0, 2.0) / 2.0;
	}
}

/**
 * @fn static void map_up_down(t_tuple point, t_uv_coord *uv_coord)
 * @brief Processes 2D wrapping for the Y-axis faces.
 * @details Maps the 3D local coordinates of the +Y and -Y faces of a cube down 
 * to 2D U/V space, adjusting bounds to fit strictly within the [0.0, 1.0] 
 * scale.
 * @param point    The local coordinate.
 * @param uv_coord Pointer to the uv data struct.
 */
static void	map_up_down(t_tuple point, t_uv_coord *uv_coord)
{
	if (uv_coord->cube_face == FACE_UP)
	{
		uv_coord->u = fmod(1.0 - point.x, 2.0) / 2.0;
		uv_coord->v = fmod(1.0 - point.z, 2.0) / 2.0;
	}
	else
	{
		uv_coord->u = fmod(1.0 - point.x, 2.0) / 2.0;
		uv_coord->v = fmod(point.z + 1.0, 2.0) / 2.0;
	}
}

/**
 * @fn static void map_front_back(t_tuple point, t_uv_coord *uv_coord)
 * @brief Processes 2D wrapping for the Z-axis faces.
 * @details Maps the 3D local coordinates of the +Z and -Z faces of a cube down 
 * to 2D U/V space, adjusting bounds to fit strictly within the [0.0, 1.0] 
 * scale.
 * @param point    The local coordinate.
 * @param uv_coord Pointer to the uv data struct.
 */
static void	map_front_back(t_tuple point, t_uv_coord *uv_coord)
{
	if (uv_coord->cube_face == FACE_FRONT)
	{
		uv_coord->u = fmod(point.x + 1.0, 2.0) / 2.0;
		uv_coord->v = fmod(point.y + 1.0, 2.0) / 2.0;
	}
	else
	{
		uv_coord->u = fmod(1.0 - point.x, 2.0) / 2.0;
		uv_coord->v = fmod(point.y + 1.0, 2.0) / 2.0;
	}
}

/**
 * @fn void cubical_map(t_tuple point, t_uv_coord *uv_coord)
 * @brief Calculates UV coordinates for a 6-sided box topology.
 * @details Finds the correct face, then applies a safe geometric offset 
 * algorithm bounded to the positive [0.0, 1.0] scale, completely avoiding C's 
 * fmod() negative issues.
 * @param point    The absolute point on the unit cube.
 * @param uv_coord The struct to populate.
 */
void	cubical_map(t_tuple point, t_uv_coord *uv_coord)
{
	uv_coord->cube_face = get_cube_face(point);
	if (uv_coord->cube_face == FACE_RIGHT || uv_coord->cube_face == FACE_LEFT)
		map_left_right(point, uv_coord);
	else if (uv_coord->cube_face == FACE_UP || uv_coord->cube_face == FACE_DOWN)
		map_up_down(point, uv_coord);
	else
		map_front_back(point, uv_coord);
}
