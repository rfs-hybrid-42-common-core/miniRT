/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   world.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 17:30:11 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 04:02:33 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "world.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stddef.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn t_world create_world(void)
 * @brief Initializes an empty world state.
 * @details Safely zero-initializes the arrays, counts, and pointers before 
 * the parser begins filling the environment data.
 * @return A blank world structure.
 */
t_world	create_world(void)
{
	return ((t_world){NULL, NULL, 0, 0, NULL, 0.0, color(0, 0, 0)});
}

/**
 * @fn t_intersections intersect_world(t_world *world, t_ray *ray)
 * @brief Master intersection router for a given ray.
 * @details This function elegantly handles the split architecture of the 
 * engine. First, it delegates the ray to the highly optimized BVH tree to 
 * check all finite objects. Then, it iteratively checks the infinite objects 
 * (like Planes) which reside at the back of the array, as infinite geometry 
 * cannot mathematically exist inside a bounding box. Finally, it sorts all 
 * recorded hits by distance.
 * @param world Pointer to the global environment state.
 * @param ray   The active ray being traced.
 * @return      A sorted container of all valid intersections.
 */
t_intersections	intersect_world(t_world *world, t_ray *ray)
{
	t_intersections	xs;
	int				i;

	xs.count = 0;
	intersect_bvh(world->bvh, &xs, ray, world->objects);
	i = world->finite_objects - 1;
	while (++i < world->total_objects && xs.count < MAX_INTERSECTIONS)
		intersect_object(&xs, ray, world->objects[i]);
	sort_intersections(&xs);
	return (xs);
}
