/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pattern_config.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/02 14:32:01 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:37:14 by maaugust         ###   ########.fr       */
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
 * @fn void set_pattern_transform(t_pattern *pattern, t_matrix *transform)
 * @brief Applies a transformation matrix to a pattern.
 * @details Precomputes the inverse matrix at parse-time. This allows the 
 * pattern to be scaled, rotated, or translated independently of the object 
 * it is painted on.
 * @param pattern   Pointer to the pattern.
 * @param transform Pointer to the new transformation matrix.
 */
void	set_pattern_transform(t_pattern *pattern, t_matrix *transform)
{
	pattern->transform = *transform;
	pattern->transform_inverse = inverse_matrix(&pattern->transform);
}

/**
 * @fn static t_color route_pattern(t_pattern *pattern, t_tuple pattern_point)
 * @brief Routes the color calculation to the specific pattern algorithm.
 * @details Uses a static jump table mapped to the pattern type enum for O(1) 
 * branchless execution during the shading loop.
 * @param pattern       The pattern being evaluated.
 * @param pattern_point The evaluated point in local pattern space.
 * @return              The calculated color at that specific coordinate.
 */
static t_color	route_pattern(t_pattern *pattern, t_tuple pattern_point)
{
	static t_color	(*get_color[])(t_pattern *, t_tuple) = {
	[STRIPE] = stripe_at,
	[GRADIENT] = gradient_at,
	[RING] = ring_at,
	[CHECKER] = checkers_at,
	[PERLIN] = perlin_at,
	[MARBLE] = marble_at,
	[UV_CHECKER] = uv_checkers_at,
	[UV_IMAGE] = uv_image_at,
	[UV_SKYBOX] = uv_skybox_at
	};

	if ((int)pattern->type < STRIPE || (int)pattern->type >= PATTTERN_COUNT
		|| !get_color[pattern->type])
		return (color(0, 0, 0));
	return (get_color[pattern->type](pattern, pattern_point));
}

/**
 * @fn t_color pattern_at_object(t_object *object, t_pattern *pattern, 
 * t_tuple world_point, double time)
 * @brief Determines the pattern color at a specific absolute world coordinate.
 * @details Performs a double-transformation. Converts the world point into 
 * local object space, and then converts the object point into local pattern 
 * space, allowing patterns to stick seamlessly to moving/deformed objects.
 * @param object      The object the pattern belongs to.
 * @param pattern     The pattern itself.
 * @param world_point The absolute intersection coordinate in the world.
 * @param time        Temporal parameter for motion blur interpolation.
 * @return            The final sampled color.
 */
t_color	pattern_at_object(t_object *object, t_pattern *pattern,
	t_tuple world_point, double time)
{
	t_tuple	object_point;
	t_tuple	pattern_point;

	object_point = world_to_object(object, world_point, time);
	pattern_point = multiply_matrix_tuple(&pattern->transform_inverse,
			object_point);
	return (route_pattern(pattern, pattern_point));
}
