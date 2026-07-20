/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bump_mapping.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 02:18:11 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 21:26:24 by maaugust         ###   ########.fr       */
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
 * @fn static t_tuple extract_normal_vector(t_computations *comps)
 * @brief Decodes a Tangent Space normal from an RGB color.
 * @details Reads the color from the normal map image. Maps the RGB values from 
 * the [0.0, 1.0] range into the [-1.0, 1.0] vector range. Applies the 
 * material's normal intensity multiplier to the X and Y components to flatten 
 * or deepen the perceived bumpiness.
 * @param comps Precomputed surface geometry.
 * @return      The decoded local tangent-space normal vector.
 */
static t_tuple	extract_normal_vector(t_computations *comps)
{
	t_color	pixel;
	t_tuple	normal;

	pixel = pattern_at_object(comps->object, comps->object->material.normal_map,
			comps->over_point, comps->time);
	normal.x = ((pixel.r * 2.0) - 1.0)
		* comps->object->material.normal_intensity;
	normal.y = ((pixel.g * 2.0) - 1.0)
		* comps->object->material.normal_intensity;
	normal.z = (pixel.b * 2.0) - 1.0;
	normal.w = 0.0;
	return (normalize(normal));
}

/**
 * @fn static double get_height(t_object *object, t_pattern *map, 
 * t_tuple point, double time)
 * @brief Samples a grayscale height value from a bump map.
 * @details Averages the R, G, and B channels of the texture to derive a single 
 * scalar height value [0.0 to 1.0]. Used as the basis for procedural bump 
 * mapping.
 * @param object The surface object.
 * @param map    The procedural pattern or image map.
 * @param point  The 3D coordinate to sample.
 * @param time   Temporal motion blur parameter.
 * @return       The scalar height value.
 */
static double	get_height(t_object *object, t_pattern *map, t_tuple point,
	double time)
{
	t_color	c;

	c = pattern_at_object(object, map, point, time);
	return ((c.r + c.g + c.b) / 3.0);
}

/**
 * @fn static t_tuple apply_blended_bump(t_computations *comps, 
 * t_tuple base_normal)
 * @brief Perturbs a normal using the Central Difference numerical method.
 * @details Estimates the surface gradient (slope) by sampling the height map 
 * at the current point, and comparing it to samples taken slightly (epsilon) 
 * along the X, Y, and Z axes. The resulting gradient vector is subtracted from 
 * the base normal to fake geometric depth.
 * @param comps       Precomputed surface geometry.
 * @param base_normal The original geometric or normal-mapped normal.
 * @return            The final perturbed bump normal.
 */
static t_tuple	apply_blended_bump(t_computations *comps, t_tuple base_normal)
{
	double	base;
	t_tuple	grad;

	base = get_height(comps->object, comps->object->material.bump_map,
			comps->over_point, comps->time);
	grad.x = (get_height(comps->object, comps->object->material.bump_map,
				add_tuples(comps->over_point,
					vector(comps->object->material.bump_epsilon, 0, 0)),
				comps->time) - base) / comps->object->material.bump_epsilon;
	grad.y = (get_height(comps->object, comps->object->material.bump_map,
				add_tuples(comps->over_point,
					vector(0, comps->object->material.bump_epsilon, 0)),
				comps->time) - base) / comps->object->material.bump_epsilon;
	grad.z = (get_height(comps->object, comps->object->material.bump_map,
				add_tuples(comps->over_point,
					vector(0, 0, comps->object->material.bump_epsilon)),
				comps->time) - base) / comps->object->material.bump_epsilon;
	grad.w = 0.0;
	grad = scaling_tuple(grad, comps->object->material.bump_strength);
	return (normalize(subtract_tuples(base_normal, grad)));
}

/**
 * @fn static t_tuple unsupported_tbn(t_computations *comps, t_tuple map_normal)
 * @brief Fallback function for objects that lack a defined Tangent Space.
 * @details Groups and CSG nodes are logical containers, not physical meshes, 
 * so they cannot generate a Tangent-Bitangent-Normal matrix. This safely 
 * bypasses normal mapping for them.
 * @param comps      Precomputed surface geometry.
 * @param map_normal The decoded normal map vector.
 * @return           The unmodified original geometric normal.
 */
static t_tuple	unsupported_tbn(t_computations *comps, t_tuple map_normal)
{
	(void)map_normal;
	return (comps->normalv);
}

/**
 * @fn t_tuple perturb_normal(t_computations *comps)
 * @brief Master router for Tangent Space Normal and Bump mapping.
 * @details Checks if the material has an assigned normal map or bump map. If a 
 * normal map exists, it decodes the texture and routes it to the specific 
 * object's TBN matrix calculator. If a bump map exists, it applies central 
 * difference perturbation to the result.
 * @param comps Precomputed surface geometry.
 * @return      The final, highly detailed surface normal used for lighting.
 */
t_tuple	perturb_normal(t_computations *comps)
{
	t_tuple			current_normal;
	t_tuple			map_normal;
	static t_tuple	(*apply_tbn[])(t_computations *, t_tuple) = {
	[SPHERE] = apply_sphere_tbn,
	[PLANE] = apply_plane_tbn,
	[CUBE] = apply_cube_tbn,
	[CYLINDER] = apply_cylinder_tbn,
	[CONE] = apply_cone_tbn,
	[TORUS] = apply_torus_tbn,
	[TRIANGLE] = apply_triangle_tbn,
	[SMOOTH_TRIANGLE] = apply_smooth_triangle_tbn,
	[GROUP] = unsupported_tbn,
	[CSG] = unsupported_tbn
	};

	current_normal = comps->normalv;
	if (comps->object->material.normal_map && comps->object->type >= SPHERE
		&& comps->object->type < OBJECT_COUNT && apply_tbn[comps->object->type])
	{
		map_normal = extract_normal_vector(comps);
		current_normal = apply_tbn[comps->object->type](comps, map_normal);
	}
	if (comps->object->material.bump_map)
		current_normal = apply_blended_bump(comps, current_normal);
	return (current_normal);
}
