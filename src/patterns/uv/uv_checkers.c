/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uv_checkers.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 02:25:40 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:56:10 by maaugust         ###   ########.fr       */
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
 * @fn t_pattern uv_checkers_pattern(int width, int height, t_uv_map map_type, 
 * t_color *colors)
 * @brief Initializes a 2D checkerboard pattern intended to wrap a 3D object.
 * @details Defines a checkerboard specifically mapped to 2D UV coordinates 
 * rather than 3D spatial coordinates. The map_type dictates how a 3D object 
 * mathematically unwraps into this 2D grid.
 * @param width    Horizontal tiles per 1.0 U coordinate.
 * @param height   Vertical tiles per 1.0 V coordinate.
 * @param map_type The algorithm to use for wrapping.
 * @param colors   Array holding the two checker colors.
 * @return         The configured pattern struct.
 */
t_pattern	uv_checkers_pattern(int width, int height, t_uv_map map_type,
	t_color *colors)
{
	t_pattern	pattern;
	t_matrix	identity;

	identity = identity_matrix();
	pattern = (t_pattern){UV_CHECKER, colors[0], colors[1], identity, identity,
	{{0}}};
	pattern.u_data.s_uv.uv_map = map_type;
	pattern.u_data.s_uv.width = width;
	pattern.u_data.s_uv.height = height;
	return (pattern);
}

/**
 * @fn static t_color uv_pattern_at(t_pattern *pattern, t_uv_coord uv_coord)
 * @brief Evaluates the checkerboard based on direct U/V coordinates.
 * @details Uses the calculated 2D U and V coordinates, scales them by the 
 * defined pattern width and height, and checks the parity of their floored sum 
 * to determine the alternating color box.
 * @param pattern  The checker pattern.
 * @param uv_coord The evaluated mapping.
 * @return         The corresponding color box.
 */
static t_color	uv_pattern_at(t_pattern *pattern, t_uv_coord uv_coord)
{
	int	u2;
	int	v2;

	u2 = floor(uv_coord.u * pattern->u_data.s_uv.width + EPSILON);
	v2 = floor(uv_coord.v * pattern->u_data.s_uv.height + EPSILON);
	if ((u2 + v2) % 2 == 0)
		return (pattern->color_a);
	return (pattern->color_b);
}

/**
 * @fn t_color uv_checkers_at(t_pattern *pattern, t_tuple pattern_point)
 * @brief Entry point for a wrapped 2D checkerboard.
 * @details Triggers the target UV mapping logic before extracting the color.
 * @param pattern       The checker pattern.
 * @param pattern_point The local 3D point.
 * @return              The determined checker color.
 */
t_color	uv_checkers_at(t_pattern *pattern, t_tuple pattern_point)
{
	t_uv_coord	uv_coord;

	uv_coord.u = 0.0;
	uv_coord.v = 0.0;
	uv_coord.cube_face = 0;
	if (pattern->u_data.s_uv.uv_map == SPHERICAL)
		spherical_map(pattern_point, &uv_coord);
	if (pattern->u_data.s_uv.uv_map == PLANAR)
		planar_map(pattern_point, &uv_coord);
	if (pattern->u_data.s_uv.uv_map == CUBICAL)
		cubical_map(pattern_point, &uv_coord);
	if (pattern->u_data.s_uv.uv_map == CYLINDRICAL)
		cylindrical_map(pattern_point, &uv_coord);
	if (pattern->u_data.s_uv.uv_map == CONICAL)
		conical_map(pattern_point, &uv_coord);
	if (pattern->u_data.s_uv.uv_map == TOROIDAL)
		toroidal_map(pattern_point, &uv_coord);
	return (uv_pattern_at(pattern, uv_coord));
}
