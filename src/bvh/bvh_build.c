/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_build.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 03:37:09 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/02 22:15:30 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "bvh.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stddef.h>
#include <stdlib.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static t_bounds get_array_bounds(t_object **objects, int start, int end)
 * @brief Calculates the total bounding box for a slice of objects.
 * @details Iterates through the specified range of objects in the array and 
 * expands an empty bounding box to completely encapsulate all of them.
 * @param objects The flattened array of world objects.
 * @param start   The starting index of the slice.
 * @param end     The ending index of the slice.
 * @return        A t_bounds struct representing the master bounding box.
 */
static t_bounds	get_array_bounds(t_object **objects, int start, int end)
{
	t_bounds	box;
	t_bounds	tmp;
	int			i;

	box = empty_bounding_box();
	i = start - 1;
	while (++i < end)
	{
		tmp = parent_space_bounds_of(objects[i]);
		add_box_to_bounds(&box, &tmp);
	}
	return (box);
}

/**
 * @fn static int get_longest_axis(t_bounds box, double *midpoint)
 * @brief Determines the longest axis of a bounding box.
 * @details Calculates the spatial extents of the box in X, Y, and Z to find 
 * the optimal axis for splitting.
 * @param box      The bounding box to measure.
 * @param midpoint Pointer to store the calculated spatial midpoint.
 * @return         0 for X, 1 for Y, or 2 for Z.
 */
static int	get_longest_axis(t_bounds box)
{
	t_tuple	extent;

	extent = subtract_tuples(box.max, box.min);
	if (extent.x > extent.y && extent.x > extent.z)
		return (0);
	if (extent.y > extent.x && extent.y > extent.z)
		return (1);
	return (2);
}

/**
 * @fn static void build_node(t_bvh *bvh, t_object **objects, int node_index, 
 * t_bvh_range range)
 * @brief Recursively constructs nodes for the BVH tree.
 * @details Evaluates the object range. If 2 or fewer objects remain, creates 
 * a leaf node. Otherwise, partitions the objects along their longest spatial 
 * axis and spawns left and right child nodes.
 * @param bvh        Pointer to the BVH controller.
 * @param objects    The array of object pointers being partitioned.
 * @param node_index The flat-array index of the current node being built.
 * @param range      The index range of objects belonging to this node.
 */
static void	build_node(t_bvh *bvh, t_object **objects, int node_index,
	t_bvh_range range)
{
	t_bvh_node	*node;
	int			axis;
	int			split_index;

	node = &bvh->nodes[node_index];
	node->bounds = get_array_bounds(objects, range.start, range.end);
	if (range.end - range.start <= 2)
	{
		node->first_object = range.start;
		node->total_objects = range.end - range.start;
		return ;
	}
	node->total_objects = 0;
	axis = get_longest_axis(node->bounds);
	split_index = partition_objects(objects, range, axis);
	node->left_index = bvh->node_count++;
	node->right_index = bvh->node_count++;
	build_node(bvh, objects, node->left_index,
		(t_bvh_range){range.start, split_index});
	build_node(bvh, objects, node->right_index,
		(t_bvh_range){split_index, range.end});
}

/**
 * @fn t_bvh *build_bvh(t_object **objects, int total_objects)
 * @brief Constructs a Bounding Volume Hierarchy from an array of objects.
 * @details Recursively partitions the objects along their longest axis and 
 * builds a flat-array tree to drastically optimize ray intersection tests.
 * @param objects       Array of pointers to the finite objects in the scene.
 * @param total_objects The number of objects to include in the BVH.
 * @return              A pointer to the dynamically allocated BVH, or NULL on 
 * failure.
 */
t_bvh	*build_bvh(t_object **objects, int total_objects)
{
	t_bvh	*bvh;

	bvh = malloc(sizeof(t_bvh));
	if (!bvh || total_objects == 0)
		return (NULL);
	bvh->node_count = 1;
	bvh->nodes_allocated = total_objects * 2;
	bvh->nodes = malloc(sizeof(t_bvh_node) * bvh->nodes_allocated);
	if (!bvh->nodes)
	{
		free(bvh);
		return (NULL);
	}
	build_node(bvh, objects, 0, (t_bvh_range){0, total_objects});
	return (bvh);
}
