/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_sampling.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 18:21:22 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/13 16:05:38 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "camera.h"
#include "render.h"
#include "shading.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stdint.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static double get_aa_jitter(double *jitter, int *pos, int seed)
 * @brief Retrieves a deterministic sub-pixel offset for Spatial Anti-Aliasing.
 * @details Employs a spatial hashing algorithm using the pixel coordinates and 
 * the sub-sample index to guarantee that jagged geometry edges receive unique, 
 * stable sampling offsets across the screen.
 * @param jitter Precomputed jitter array.
 * @param pos    Array containing the X and Y screen coordinates.
 * @param seed   The specific sub-sample index modifier.
 * @return       The floating-point noise offset.
 */
static double	get_aa_jitter(double *jitter, int *pos, int seed)
{
	int	index;

	index = pos[0] * 31 + pos[1] * 73 + seed * 101;
	if (index < 0)
		index = -index;
	return (jitter[index % JITTER_SIZE]);
}

/**
 * @fn static t_ray get_aa_ray(t_engine *engine, int *pos, int i, uint32_t *rng)
 * @brief Generates a slightly offset camera ray based on Anti-Aliasing levels.
 * @details Separates spatial sub-pixel jitter from aperture lens jitter. If 
 * focal blur is active, it generates a random offset across the lens opening 
 * completely independent of the spatial anti-aliasing grid.
 * @param engine Pointer to the global engine context.
 * @param pos    Array containing the X and Y screen coordinates.
 * @param i      The sub-sample index.
 * @param rng    Pointer to the local pseudo-random number generator state.
 * @return       The physically jittered ray.
 */
static t_ray	get_aa_ray(t_engine *engine, int *pos, int i, uint32_t *rng)
{
	double	aa_jitter[2];
	double	lens_offset[2];

	aa_jitter[0] = 0.0;
	aa_jitter[1] = 0.0;
	if (engine->aa_level != AA_NONE)
	{
		aa_jitter[0] = get_aa_jitter(engine->aa_jitter, pos, i * 2) - 0.5;
		aa_jitter[1] = get_aa_jitter(engine->aa_jitter, pos, i * 2 + 1) - 0.5;
	}
	lens_offset[0] = 0.0;
	lens_offset[1] = 0.0;
	if (engine->render_flags & FLAG_FOCAL_BLUR)
	{
		lens_offset[0] = random_double(rng) - 0.5;
		lens_offset[1] = random_double(rng) - 0.5;
	}
	return (ray_for_pixel(engine,
			pos[0] + ((i % engine->aa_level) + aa_jitter[0] + 0.5)
			/ engine->aa_level,
			pos[1] + ((i / engine->aa_level) + aa_jitter[1] + 0.5)
			/ engine->aa_level,
			lens_offset));
}

/**
 * @fn static t_color evaluate_ray(t_engine *engine, int *pos, int i, 
 * uint32_t *rng)
 * @brief Encapsulates the ray generation and color evaluation phase.
 * @details Extracted to bypass the 25-line limit in the main sampling loop. 
 * Conditionally applies randomized temporal offsets if motion blur is active.
 * @param engine Pointer to the global engine context.
 * @param pos    Array holding the active X and Y coordinates.
 * @param i      The sub-sample index.
 * @param rng    Pointer to the local pseudo-random number generator state.
 * @return       The evaluated color for the specific ray.
 */
static t_color	evaluate_ray(t_engine *engine, int *pos, int i,
	uint32_t *rng)
{
	t_ray	ray;

	ray = get_aa_ray(engine, pos, i, rng);
	if (engine->render_flags & FLAG_MOTION_BLUR)
		ray.time = random_double(rng);
	else
		ray.time = 0.0;
	return (color_at(&engine->world, &ray, MAX_RECURSION));
}

/**
 * @fn static bool is_variance_high(t_color *c)
 * @brief Checks if a set of color samples crosses a variance threshold.
 * @details Finds the absolute minimum and maximum values across the R, G, 
 * and B channels among the 4 provided corner samples. If the difference 
 * between the brightest and darkest sample exceeds 0.05 (~12 RGB values), 
 * it flags the pixel as visually volatile (e.g., sitting on an edge or 
 * shadow boundary).
 * @param c Array of 4 evaluated colors.
 * @return  True if the pixel needs further sampling, false if it is uniform.
 */
static bool	is_variance_high(t_color *c)
{
	t_color	min_c;
	t_color	max_c;
	int		i;

	min_c = c[0];
	max_c = c[0];
	i = 0;
	while (++i < 4)
	{
		if (c[i].r < min_c.r)
			min_c.r = c[i].r;
		if (c[i].g < min_c.g)
			min_c.g = c[i].g;
		if (c[i].b < min_c.b)
			min_c.b = c[i].b;
		if (c[i].r > max_c.r)
			max_c.r = c[i].r;
		if (c[i].g > max_c.g)
			max_c.g = c[i].g;
		if (c[i].b > max_c.b)
			max_c.b = c[i].b;
	}
	return (max_c.r - min_c.r > 0.05 || max_c.g - min_c.g > 0.05
		|| max_c.b - min_c.b > 0.05);
}

/**
 * @fn t_color sample_pixel(t_engine *engine, int x, int y)
 * @brief Adaptive master sampling router.
 * @details Evaluates the 4 extreme corners of the sub-pixel grid. If the 
 * variance is low, it returns the 4-sample average. If the variance is 
 * high, it triggers the remaining interior rays. The `pos` array caches 
 * `[x, y, total_rays]`.
 * @param engine Pointer to the global engine context.
 * @param x      X screen coordinate.
 * @param y      Y screen coordinate.
 * @return       The final, adaptively smoothed color.
 */
t_color	sample_pixel(t_engine *engine, int x, int y)
{
	t_color		accum;
	t_color		c[4];
	uint32_t	rng;
	int			pos[3];
	int			i;

	pos[0] = x;
	pos[1] = y;
	pos[2] = engine->aa_level * engine->aa_level;
	rng = x * 73 + y * 191 + 12345;
	if (engine->aa_level == AA_NONE)
		return (evaluate_ray(engine, pos, 0, &rng));
	c[0] = evaluate_ray(engine, pos, 0, &rng);
	c[1] = evaluate_ray(engine, pos, pos[2] - 1, &rng);
	c[2] = evaluate_ray(engine, pos, engine->aa_level - 1, &rng);
	c[3] = evaluate_ray(engine, pos, pos[2] - engine->aa_level, &rng);
	accum = add_colors(add_colors(c[0], c[1]), add_colors(c[2], c[3]));
	if (engine->aa_level == AA_X4 || !is_variance_high(c))
		return (scaling_color(accum, 0.25));
	i = 0;
	while (++i < pos[2] - 1)
		if (i != (int)engine->aa_level - 1
			&& i != pos[2] - (int)engine->aa_level)
			accum = add_colors(accum, evaluate_ray(engine, pos, i, &rng));
	return (scaling_color(accum, 1.0 / pos[2]));
}
