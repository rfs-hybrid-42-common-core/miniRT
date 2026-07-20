/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   light_properties.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 14:20:33 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 17:54:23 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "shading.h"

/* --------------------------- External Libraries --------------------------- */
#include <stddef.h>
#include <stdlib.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn t_light *create_point_light(t_tuple position, t_color intensity)
 * @brief Allocates and initializes a standard 0-dimensional point light.
 * @details Point lights shoot rays infinitely in all directions and cast hard, 
 * sharp shadows. Unused spot and area parameters are zeroed out.
 * @param position  The absolute spatial location.
 * @param intensity The RGB color output.
 * @return          Pointer to the initialized light.
 */
t_light	*create_point_light(t_tuple position, t_color intensity)
{
	t_light	*light;

	light = malloc(sizeof(t_light));
	if (!light)
		return (NULL);
	light->type = LIGHT_POINT;
	light->position = position;
	light->direction = vector(0, 0, 0);
	light->inner_cutoff = 0.0;
	light->outer_cutoff = 0.0;
	light->intensity = intensity;
	light->uvec = vector(0, 0, 0);
	light->usteps = 0;
	light->vvec = vector(0, 0, 0);
	light->vsteps = 0;
	light->samples = 1;
	light->center = position;
	light->jitter = NULL;
	light->jitter_size = 0;
	return (light);
}

/**
 * @fn t_light *create_area_light(t_tuple corner, t_tuple *full_uv, 
 * int *uvsteps, t_color intensity)
 * @brief Allocates and initializes a 2D rectangular area light.
 * @details Creates a grid of sub-lights to simulate soft shadows (penumbras). 
 * Divides the full U and V vectors by their respective steps to define the 
 * size of a single sub-cell, and spawns the jitter sequence for randomization.
 * @param corner    The spatial origin (bottom-left) of the rectangle.
 * @param full_uv   Array containing the full U (width) and V (height) vectors.
 * @param uvsteps   Array containing the amount of cells in the U and V axes.
 * @param intensity The overall RGB color output.
 * @return          Pointer to the initialized area light.
 */
t_light	*create_area_light(t_tuple corner, t_tuple *full_uv, int *uvsteps,
	t_color intensity)
{
	t_light	*light;

	light = malloc(sizeof(t_light));
	if (!light)
		return (NULL);
	light->type = LIGHT_AREA;
	light->position = corner;
	light->direction = vector(0, 0, 0);
	light->inner_cutoff = 0.0;
	light->outer_cutoff = 0.0;
	light->intensity = intensity;
	light->uvec = scaling_tuple(full_uv[0], 1.0 / (double)uvsteps[0]);
	light->usteps = uvsteps[0];
	light->vvec = scaling_tuple(full_uv[1], 1.0 / (double)uvsteps[1]);
	light->vsteps = uvsteps[1];
	light->samples = uvsteps[0] * uvsteps[1];
	light->center = add_tuples(corner, add_tuples(
				scaling_tuple(full_uv[0], 0.5),
				scaling_tuple(full_uv[1], 0.5)));
	light->jitter = generate_jitter_sequence(JITTER_SIZE);
	light->jitter_size = JITTER_SIZE;
	return (light);
}

/**
 * @fn t_tuple sample_area_light(t_light *light, t_tuple point, int u, int v)
 * @brief Evaluates a specific grid cell on an area light.
 * @details Retrieves a randomized jitter factor to offset the sample point 
 * slightly within the specified cell. This Stratified Jittered Sampling 
 * breaks up unnatural banding patterns in the resulting shadow penumbra.
 * @param light The area light source.
 * @param point The surface coordinate being shaded.
 * @param u     The horizontal cell index.
 * @param v     The vertical cell index.
 * @return      The randomly offset light-emitting coordinate.
 */
t_tuple	sample_area_light(t_light *light, t_tuple point, int u, int v)
{
	double	jitter_u;
	double	jitter_v;
	t_tuple	u_offset;
	t_tuple	v_offset;

	jitter_u = get_jitter(light, point, u, v);
	jitter_v = get_jitter(light, point, u + 17, v + 23);
	u_offset = scaling_tuple(light->uvec, (u + jitter_u));
	v_offset = scaling_tuple(light->vvec, (v + jitter_v));
	return (add_tuples(light->position, add_tuples(u_offset, v_offset)));
}
