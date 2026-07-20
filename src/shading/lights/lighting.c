/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lighting.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 19:06:54 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 17:56:04 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "patterns.h"
#include "shading.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static t_color get_base_color(t_computations *comps)
 * @brief Retrieves the foundational color of the object material.
 * @details Routes to the procedural pattern/texture mapping pipeline if the 
 * material has an active pattern pointer. Otherwise, falls back to the solid 
 * RGB.
 * @param comps Precomputed intersection data.
 * @return      The base unlit color.
 */
static t_color	get_base_color(t_computations *comps)
{
	if (!comps->object->material.pattern)
		return (comps->object->material.color);
	return (pattern_at_object(comps->object,
			comps->object->material.pattern, comps->point, comps->time));
}

/**
 * @fn static double get_spotlight_attenuation(t_light *light, t_tuple lightv)
 * @brief Calculates the soft cone falloff for spotlights.
 * @details Uses dot products to determine the ray's angle relative to the 
 * center of the spotlight cone. Interpolates smoothly between 1.0 (inside 
 * inner cone) and 0.0 (outside outer cone) using a quartic easing function.
 * @param light  The spotlight source.
 * @param lightv Normalized vector pointing from hit to the light.
 * @return       Multiplier for light intensity (0.0 to 1.0).
 */
static double	get_spotlight_attenuation(t_light *light, t_tuple lightv)
{
	double	theta;
	double	fraction;

	theta = dot(negating_tuple(lightv), light->direction);
	if (theta >= light->inner_cutoff)
		return (1.0);
	if (theta <= light->outer_cutoff)
		return (0.0);
	fraction = (theta - light->outer_cutoff)
		/ (light->inner_cutoff - light->outer_cutoff);
	return (fraction * fraction * fraction * fraction);
}

/**
 * @fn static t_color get_specular(t_computations *comps, t_light *light, 
 * t_tuple lightv)
 * @brief Calculates the Blinn-Phong specular reflection highlight.
 * @details Determines if the reflected light vector aligns with the eye/camera 
 * vector. The highlight is sharpened exponentially based on the material's 
 * shininess parameter.
 * @param comps  Precomputed geometry.
 * @param light  The evaluated light.
 * @param lightv Normalized vector towards the light.
 * @return       The isolated RGB specular contribution.
 */
static t_color	get_specular(t_computations *comps, t_light *light,
	t_tuple lightv)
{
	t_color	specular;
	t_tuple	reflectv;
	double	reflect_dot_eye;
	double	factor;

	reflectv = reflect(negating_tuple(lightv), comps->normalv);
	reflect_dot_eye = dot(reflectv, comps->eyev);
	if (reflect_dot_eye <= -EPSILON)
		specular = color(0, 0, 0);
	else
	{
		factor = pow(reflect_dot_eye, comps->object->material.shininess);
		specular = scaling_color(light->intensity,
				comps->object->material.specular * factor);
	}
	return (specular);
}

/**
 * @fn t_color ambient_lighting(t_world *world, t_computations *comps)
 * @brief Computes the baseline global illumination for a surface.
 * @details Multiplies the object's base color by the world's ambient light 
 * ratio. Allows emissive/glowing objects (ambient=1.0) to bypass shadows 
 * entirely.
 * @param world Pointer to the global environment.
 * @param comps Precomputed surface geometry.
 * @return      The baseline ambient color contribution.
 */
t_color	ambient_lighting(t_world *world, t_computations *comps)
{
	t_color	base_color;
	t_color	global_ambient;

	base_color = get_base_color(comps);
	if (comps->object->material.ambient >= 1.0 - EPSILON)
		return (base_color);
	if (comps->object->material.ambient <= EPSILON)
		return (color(0, 0, 0));
	global_ambient = scaling_color(world->ambient_color, world->ambient_ratio);
	return (multiply_colors(base_color, global_ambient));
}

/**
 * @fn t_color lighting(t_light *light, t_computations *comps, double intensity)
 * @brief Master physical lighting equation.
 * @details Implements the diffuse calculation (Lambert's Cosine Law) and sums 
 * it with the specular contribution. Everything is scaled by the incoming 
 * intensity (which acts as the shadow/penumbra multiplier).
 * @param light     The active light source.
 * @param comps     The precomputed geometry variables.
 * @param intensity The pre-evaluated occlusion multiplier.
 * @return          The combined diffuse and specular lighting contribution.
 */
t_color	lighting(t_light *light, t_computations *comps, double intensity)
{
	t_color	diffuse;
	t_color	specular;
	t_color	effective_color;
	t_tuple	lightv;
	double	light_dot_normal;

	if (intensity < EPSILON)
		return (color(0, 0, 0));
	effective_color = multiply_colors(get_base_color(comps), light->intensity);
	lightv = normalize(subtract_tuples(light->position, comps->point));
	if (light->type == LIGHT_SPOT)
	{
		intensity *= get_spotlight_attenuation(light, lightv);
		if (intensity < EPSILON)
			return (color(0, 0, 0));
	}
	light_dot_normal = dot(lightv, comps->normalv);
	if (light_dot_normal < -EPSILON)
		return (color(0, 0, 0));
	diffuse = scaling_color(scaling_color(effective_color,
				comps->object->material.diffuse * light_dot_normal), intensity);
	specular = scaling_color(get_specular(comps, light, lightv), intensity);
	return (add_colors(diffuse, specular));
}
