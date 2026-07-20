/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   world.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 20:10:10 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 04:01:25 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WORLD_H
# define WORLD_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "bvh.h"
# include "libft.h"
# include "math_rt.h"
# include "objects.h"
# include "ray.h"

/* ========================================================================== */
/* TYPEDEFS & DATA STRUCTURES                                                 */
/* ========================================================================== */

/**
 * @typedef t_engine
 * @brief Forward declaration of the core engine structure.
 * @details Resolves circular dependencies between world.h and engine.h.
 */
typedef struct s_engine			t_engine;

/**
 * @struct s_world
 * @brief The master container for the physical scene state.
 * @details Holds all lighting, parsed geometry, and spatial partitioning data 
 * required to render the scene. The objects are stored in a contiguous flat 
 * array rather than a linked list to maximize CPU cache coherency.
 * @var lights         Linked list of all active light sources in the scene.
 * @var objects        Flat array containing every renderable primitive.
 * @var total_objects  The absolute size of the objects array.
 * @var finite_objects The count of objects with closed boundaries 
 * (e.g., Spheres).
 * @var bvh            Pointer to the root node of the Bounding Volume 
 * Hierarchy.
 * @var ambient_ratio  The intensity multiplier for global illumination.
 * @var ambient_color  The baseline RGB color illuminating the entire scene.
 */
typedef struct s_world
{
	t_list		*lights;
	t_object	**objects;
	int			total_objects;
	int			finite_objects;
	t_bvh		*bvh;
	double		ambient_ratio;
	t_color		ambient_color;
}	t_world;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

t_world			create_world(void);
t_intersections	intersect_world(t_world *world, t_ray *ray);
void			flatten_world(t_world *world, t_list *parsed_list,
					t_engine *engine);

#endif
