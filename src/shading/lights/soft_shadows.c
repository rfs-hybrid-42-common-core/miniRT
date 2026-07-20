/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   soft_shadows.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 03:22:16 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/03 22:13:59 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "bvh.h"
#include "shading.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static bool is_shadowed(t_world *world, t_tuple over_point, 
 * t_tuple light_point, double time)
 * @brief Casts a shadow ray to determine if a specific point is occluded.
 * @details Defers to the highly optimized bvh_is_occluded() function, which 
 * short-circuits and returns true the instant ANY geometry is found between 
 * the surface and the light source.
 * @param world       The world state containing the BVH tree.
 * @param over_point  The lifted origin surface coordinate.
 * @param light_point The specific coordinate on the light being tested.
 * @param time        Temporal parameter for motion-blurred shadows.
 * @return            True if blocked, false if illuminated.
 */
static bool	is_shadowed(t_world *world, t_tuple over_point, t_tuple light_point,
	double time)
{
	t_tuple	vector;
	t_ray	ray;

	vector = subtract_tuples(light_point, over_point);
	ray = create_ray(over_point, normalize(vector), time);
	return (bvh_is_occluded(world->bvh, &ray, magnitude(vector),
			world->objects));
}

/**
 * @fn static t_tuple get_sparse_point(t_light *light, int u, int v)
 * @brief Calculates the exact 3D coordinate for an optimization test point.
 * @details If the light has >= 4 steps, it spreads the 4 test points evenly 
 * across the grid (0%, 33%, 66%, 100%). If it has < 4 steps, it falls back 
 * to checking the exact cell indices.
 * @param light The area light being tested.
 * @param u     The horizontal test index (0 to 3).
 * @param v     The vertical test index (0 to 3).
 * @return      The 3D coordinate on the light's surface.
 */
static t_tuple	get_sparse_point(t_light *light, int u, int v)
{
	double	u_mult;
	double	v_mult;

	u_mult = u;
	if (light->usteps >= 4)
		u_mult = (light->usteps / 3.0) * u;
	v_mult = v;
	if (light->vsteps >= 4)
		v_mult = (light->vsteps / 3.0) * v;
	return (add_tuples(light->position, add_tuples(
				scaling_tuple(light->uvec, u_mult),
				scaling_tuple(light->vvec, v_mult))));
}

/**
 * @fn static int check_sparse_grid(t_world *world, t_tuple over_point, 
 * t_light *light, double time)
 * @brief Performs a dynamic adaptive sparse grid optimization test.
 * @details Evaluates a sparse 16-point grid across the area light to quickly 
 * identify fully occluded or fully illuminated regions. Dynamically scales 
 * down to exact-cell checking if the light has fewer than 4 steps on either 
 * axis, preventing redundant ray calculations on narrow lights.
 * @return 0 for pure shadow, 1 for fully lit, -1 for mixed penumbra.
 */
static int	check_sparse_grid(t_world *world, t_tuple over_point,
	t_light *light, double time)
{
	bool	first;
	int		u;
	int		v;

	first = is_shadowed(world, over_point, light->position, time);
	u = -1;
	while (++u < 4 && (light->usteps >= 4 || u < light->usteps))
	{
		v = -1;
		while (++v < 4 && (light->vsteps >= 4 || v < light->vsteps))
			if (is_shadowed(world, over_point,
					get_sparse_point(light, u, v), time) != first)
				return (-1);
	}
	if (first)
		return (0);
	return (1);
}

/**
 * @fn static double calculate_penumbra(t_world *world, t_tuple over_point, 
 * t_light *light, double time)
 * @brief Accumulates soft shadow intensity from an area light grid.
 * @details Loops through every sub-cell of the area light, casts a jittered 
 * shadow ray, and divides the successful hits by the total sample count.
 * @param world      The global world state.
 * @param over_point The surface coordinate being shaded.
 * @param light      The area light.
 * @param time       Temporal parameter for motion blur.
 * @return           A fractional multiplier representing the penumbra darkness.
 */
static double	calculate_penumbra(t_world *world, t_tuple over_point,
	t_light *light, double time)
{
	double	total_light;
	t_tuple	light_point;
	int		u;
	int		v;

	total_light = 0.0;
	v = -1;
	while (++v < light->vsteps)
	{
		u = -1;
		while (++u < light->usteps)
		{
			light_point = sample_area_light(light, over_point, u, v);
			if (!is_shadowed(world, over_point, light_point, time))
				total_light += 1.0;
		}
	}
	return (total_light / light->samples);
}

/**
 * @fn double intensity_at(t_world *world, t_tuple over_point, t_light *light, 
 * double time)
 * @brief Determines the fractional visibility of a light source.
 * @details Serves as the master shadow router. If the area light is small 
 * (<= SPARSE_THRESHOLD), it brute-forces it for speed. If large, it triggers 
 * the sparse 16-point grid optimization to safely save performance without 
 * banding artifacts.
 * @param world      The global world state.
 * @param over_point The surface coordinate being tested.
 * @param light      The active light source.
 * @param time       Temporal parameter for motion blur.
 * @return           A float representing occlusion intensity (0.0 to 1.0).
 */
double	intensity_at(t_world *world, t_tuple over_point, t_light *light,
	double time)
{
	int	sparse;

	if (light->type == LIGHT_POINT || light->type == LIGHT_SPOT)
		return ((double) !is_shadowed(world, over_point, light->position,
				time));
	if (light->samples > 16)
	{
		sparse = check_sparse_grid(world, over_point, light, time);
		if (sparse == 0 || sparse == 1)
			return ((double)sparse);
	}
	return (calculate_penumbra(world, over_point, light, time));
}
