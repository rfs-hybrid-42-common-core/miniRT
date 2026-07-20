/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bounds_of.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 00:59:48 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 13:36:54 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "objects.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn t_bounds bounds_of(t_object *object)
 * @brief Routes to the correct bounding box calculator based on object type.
 * @details Uses a static function pointer jump table for O(1) routing.
 * @param object The target object.
 * @return       The raw, untransformed bounding box in local object space.
 */
t_bounds	bounds_of(t_object *object)
{
	static t_bounds	(*get_bounds[])(t_object *) = {
	[SPHERE] = bounds_of_sphere,
	[PLANE] = bounds_of_plane,
	[CUBE] = bounds_of_cube,
	[CYLINDER] = bounds_of_cylinder,
	[CONE] = bounds_of_cone,
	[TORUS] = bounds_of_torus,
	[TRIANGLE] = bounds_of_triangle,
	[SMOOTH_TRIANGLE] = bounds_of_smooth_triangle,
	[GROUP] = bounds_of_group,
	[CSG] = bounds_of_csg
	};

	if (object->type < SPHERE || object->type >= OBJECT_COUNT
		|| !get_bounds[object->type])
		return ((t_bounds){point(0, 0, 0), point(0, 0, 0)});
	return (get_bounds[object->type](object));
}
