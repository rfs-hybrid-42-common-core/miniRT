/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shading_pipeline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 04:10:20 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/01 21:54:16 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"
#include "shading.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static bool is_facing_light(t_computations *comps, t_light *light)
 * @brief Rapidly determines if a surface faces a light source.
 * @details Computes the dot product between the surface normal and the 
 * vector pointing to the light (using the center for area lights). If the 
 * dot product is zero or negative, the light is physically behind the surface 
 * horizon. This allows the engine to completely skip expensive shadow ray 
 * BVH traversals for back-facing geometry.
 * @param comps Precomputed surface geometry.
 * @param light The light source being evaluated.
 * @return      True if the surface points toward the light, false otherwise.
 */
static bool	is_facing_light(t_computations *comps, t_light *light)
{
	t_tuple	light_pos;
	t_tuple	lightv;

	if (light->type == LIGHT_AREA)
		light_pos = light->center;
	else
		light_pos = light->position;
	lightv = subtract_tuples(light_pos, comps->over_point);
	if (dot(lightv, comps->normalv) <= 0.0)
		return (false);
	return (true);
}

/**
 * @fn static t_color surface_color(t_world *world, t_computations *comps)
 * @brief Iterates through all light sources to calculate diffuse and specular.
 * @details Establishes the base ambient light first, then adds the physical 
 * lighting contributions for every light in the scene, automatically taking 
 * shadows and soft penumbras into account via the intensity variable.
 * @param world Pointer to the world state.
 * @param comps Precomputed surface vectors.
 * @return      The combined RGB surface color.
 */
static t_color	surface_color(t_world *world, t_computations *comps)
{
	t_light	*light;
	t_list	*current_light;
	t_color	surface;
	double	intensity;

	surface = ambient_lighting(world, comps);
	current_light = world->lights;
	while (current_light)
	{
		light = (t_light *)current_light->content;
		if (is_facing_light(comps, light))
		{
			intensity = intensity_at(world, comps->over_point, light,
					comps->time);
			surface = add_colors(surface, lighting(light, comps, intensity));
		}
		current_light = current_light->next;
	}
	return (surface);
}

/**
 * @fn t_color shade_hit(t_world *world, t_computations *comps, int remaining)
 * @brief Combines surface lighting with advanced optical effects.
 * @details Sums the standard surface color with spawned reflection and 
 * refraction rays. Crucially, if an object is both reflective AND transparent 
 * (like glass), it utilizes Schlick's approximation to properly balance how 
 * much light is mirrored vs. passed through based on the viewing angle.
 * @param world     Pointer to the world state.
 * @param comps     Precomputed geometric data.
 * @param remaining Depth counter to stop recursive bouncing.
 * @return          The final composited color.
 */
t_color	shade_hit(t_world *world, t_computations *comps, int remaining)
{
	t_color	surface;
	t_color	reflected;
	t_color	refracted;
	double	reflectance;

	surface = surface_color(world, comps);
	reflected = reflected_color(world, comps, remaining);
	refracted = refracted_color(world, comps, remaining);
	if (comps->object->material.reflective > EPSILON
		&& comps->object->material.transparency > EPSILON)
	{
		reflectance = schlick(comps);
		return (add_colors(surface,
				add_colors(scaling_color(reflected, reflectance),
					scaling_color(refracted, 1.0 - reflectance))));
	}
	return (add_colors(surface, add_colors(reflected, refracted)));
}

/**
 * @fn t_color color_at(t_world *world, t_ray *ray, int remaining)
 * @brief Master entry point for rendering a specific ray's path.
 * @details Intersects the ray with the BVH tree. If a valid hit occurs, it 
 * prepares the computation struct and routes it to the shading pipeline. If it 
 * misses all geometry, it returns pitch black (background).
 * @param world     Pointer to the world environment.
 * @param ray       The active ray being traced.
 * @param remaining Recursive depth limit.
 * @return          The resolved color.
 */
t_color	color_at(t_world *world, t_ray *ray, int remaining)
{
	t_intersections	xs;
	t_intersection	*closest;
	t_computations	comps;

	xs = intersect_world(world, ray);
	if (xs.count <= 0)
		return (color(0, 0, 0));
	closest = hit(&xs);
	if (!closest)
		return (color(0, 0, 0));
	comps = prepare_computations(closest, ray, &xs);
	return (shade_hit(world, &comps, remaining));
}
