/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   world_flatten.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 04:36:25 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 04:24:28 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "engine.h"
#include "world.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stddef.h>
#include <stdlib.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static int count_primitives(t_list *parsed_list)
 * @brief Recursively tallies all renderable geometry in the hierarchy.
 * @details Iterates through the raw parsed linked list. It explicitly ignores 
 * logical GROUP containers, diving into their children to count only the 
 * physical shape primitives (Spheres, Triangles, etc.). This ensures the final 
 * array allocates exact, minimal memory.
 * @param parsed_list The root linked list of parsed scene elements.
 * @return            The total integer count of physical geometry.
 */
static int	count_primitives(t_list *parsed_list)
{
	t_object	*object;
	int			count;

	count = 0;
	while (parsed_list)
	{
		object = (t_object *)parsed_list->content;
		if (object->type == GROUP)
			count += count_primitives(object->data.group.children);
		else
			count++;
		parsed_list = parsed_list->next;
	}
	return (count);
}

/**
 * @fn static void insert_primitive(t_object *object, t_matrix *transformed, 
 * t_world *world, int *index)
 * @brief Bakes the accumulated transformation matrix into a primitive.
 * @details Overwrites the object's local matrix with the cascaded group 
 * matrix, aggressively recalculates the necessary inverse matrices to save 
 * render time, and strips the `parent` pointer since the hierarchy is no 
 * longer needed. Crucially, it segregates the flat array: finite objects 
 * fill from the front (`index[0]`), while infinite planes fill backwards 
 * from the end (`index[1]`).
 * @param object      The primitive shape being processed.
 * @param transformed Array holding the active [Start, End] motion matrices.
 * @param world       The target world state.
 * @param index       Array holding the front [0] and back [1] insertion 
 * trackers.
 */
static void	insert_primitive(t_object *object, t_matrix *transformed,
	t_world *world, int *index)
{
	object->parent = NULL;
	object->transform = transformed[0];
	object->transform_end = transformed[1];
	object->transform_inverse = inverse_matrix(&object->transform);
	object->transform_inverse_transpose = transpose_matrix(
			&object->transform_inverse);
	object->is_moving = !equal_matrices(&object->transform,
			&object->transform_end);
	if (object->type == PLANE)
		world->objects[(index[1])--] = object;
	else
		world->objects[(index[0])++] = object;
}

/**
 * @fn static void extract(t_list *parsed_list, t_matrix *matrix, 
 * t_world *world, int *index)
 * @brief Recursively cascades group transformations down to child objects.
 * @details Transverses the tree. If it encounters a group, it multiplies the 
 * current active matrix against the group's local matrix and passes the sum 
 * deeper into the tree. If it hits a primitive, it calls `insert_primitive`. 
 * This permanently bakes spatial nesting into the physical geometry.
 * @param parsed_list The current level of the linked list tree.
 * @param matrix      Array holding the accumulated [Start, End] matrices.
 * @param world       The target world state.
 * @param index       Array tracking insertion placement.
 */
static void	extract(t_list *parsed_list, t_matrix *matrix, t_world *world,
	int *index)
{
	t_object	*object;
	t_matrix	transformed[2];

	while (parsed_list)
	{
		object = (t_object *)parsed_list->content;
		transformed[0] = multiply_matrices(&matrix[0], &object->transform);
		transformed[1] = multiply_matrices(&matrix[1], &object->transform_end);
		if (object->type == GROUP)
			extract(object->data.group.children, transformed, world, index);
		else
			insert_primitive(object, transformed, world, index);
		parsed_list = parsed_list->next;
	}
}

/**
 * @fn static void free_parsed_groups(t_list *parsed_list, void *mlx_ptr)
 * @brief Cleans up the temporary tree hierarchy and group containers.
 * @details Because the group transformations were baked directly into the 
 * child primitives, the logical group structs and the linked list nodes 
 * that held them are completely useless for rendering. This cleanly deletes 
 * them to prevent memory bloat.
 * @param parsed_list The root parsed list.
 * @param mlx_ptr     The active MiniLibX instance (for freeing inner textures).
 */
static void	free_parsed_groups(t_list *parsed_list, void *mlx_ptr)
{
	t_object	*object;
	t_list		*next;

	while (parsed_list)
	{
		next = parsed_list->next;
		object = (t_object *)parsed_list->content;
		if (object->type == GROUP)
		{
			free_parsed_groups(object->data.group.children, mlx_ptr);
			object->data.group.children = NULL;
			free_object(object, mlx_ptr);
		}
		free(parsed_list);
		parsed_list = next;
	}
}

/**
 * @fn void flatten_world(t_world *world, t_list *parsed_list, t_engine *engine)
 * @brief Transforms a slow linked-list tree into a high-performance flat array.
 * @details Master controller for the flattening process. Counts the 
 * primitives, allocates the optimized array, extracts the geometry, frees the 
 * obsolete tree, and immediately triggers the BVH spatial builder specifically 
 * on the subset of finite geometry isolated at the front of the array.
 * @param world       The target world state being finalized.
 * @param parsed_list The raw, hierarchical linked list provided by the parser.
 * @param engine      The global engine context (for error handling).
 */
void	flatten_world(t_world *world, t_list *parsed_list, t_engine *engine)
{
	t_matrix	identity[2];
	int			index[2];

	world->total_objects = count_primitives(parsed_list);
	world->objects = malloc(sizeof(t_object *) * world->total_objects);
	if (!world->objects)
	{
		free_parsed_groups(parsed_list, engine->mlx.mlx_ptr);
		exit_program(engine);
	}
	identity[0] = identity_matrix();
	identity[1] = identity[0];
	index[0] = 0;
	index[1] = world->total_objects - 1;
	extract(parsed_list, identity, world, index);
	free_parsed_groups(parsed_list, engine->mlx.mlx_ptr);
	world->finite_objects = index[0];
	world->bvh = build_bvh(world->objects, world->finite_objects);
	if (!world->bvh && world->finite_objects > 0)
		exit_program(engine);
}
