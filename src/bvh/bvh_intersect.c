/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_intersect.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 16:08:23 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/15 02:37:49 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "bvh.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static bool occluded_leaf(t_bvh_node *node, t_ray *ray, double distance, 
 * t_object **objects)
 * @brief Checks if a shadow ray is occluded by objects inside a leaf node.
 * @details Instantly returns true if ANY valid hit is detected between the 
 * ray's origin and the maximum light distance. Fully transparent objects and 
 * highly ambient (emissive) objects are bypassed, allowing shadow rays to 
 * pass through them to illuminate geometry behind them.
 * @param node     The BVH leaf node being tested.
 * @param ray      The shadow ray.
 * @param distance The maximum valid distance (distance to the light source).
 * @param objects  The master array of world objects.
 * @return         True if an occlusion is found, false otherwise.
 */
static bool	occluded_leaf(t_bvh_node *node, t_ray *ray, double distance,
	t_object **objects)
{
	t_intersections	xs;
	t_object		*object;
	int				i;
	int				j;

	i = -1;
	while (++i < node->total_objects)
	{
		object = objects[node->first_object + i];
		xs.count = 0;
		intersect_object(&xs, ray, object);
		j = -1;
		while (++j < xs.count)
			if (xs.array[j].t > EPSILON && xs.array[j].t < distance
				&& object->material.ambient < 1.0 - EPSILON
				&& object->material.transparency < EPSILON)
				return (true);
	}
	return (false);
}

/**
 * @fn bool bvh_is_occluded(t_bvh *bvh, t_ray *ray, double distance, 
 * t_object **objects)
 * @brief Quick boolean test to determine if a shadow ray is occluded.
 * @details Similar to intersect_bvh, but instantly returns true the moment 
 * ANY valid intersection is found between the ray origin and the light source.
 * @param bvh      Pointer to the BVH tree.
 * @param ray      The shadow ray pointing towards the light.
 * @param distance The distance to the light source.
 * @param objects  The flattened array of world objects.
 * @return         True if the ray hits an object before the light, false 
 * otherwise.
 */
bool	bvh_is_occluded(t_bvh *bvh, t_ray *ray, double distance,
	t_object **objects)
{
	int			nodes_to_check[BVH_MAX_DEPTH];
	int			node_count;
	t_bvh_node	*current;

	if (!bvh || bvh->node_count == 0)
		return (false);
	node_count = 0;
	nodes_to_check[node_count++] = 0;
	while (node_count > 0)
	{
		current = &bvh->nodes[nodes_to_check[--node_count]];
		if (!intersect_bounds(ray, &current->bounds))
			continue ;
		if (current->total_objects > 0)
		{
			if (occluded_leaf(current, ray, distance, objects))
				return (true);
			continue ;
		}
		if (node_count >= BVH_MAX_DEPTH - 2)
			continue ;
		nodes_to_check[node_count++] = current->left_index;
		nodes_to_check[node_count++] = current->right_index;
	}
	return (false);
}

/**
 * @fn static void intersect_leaf(t_bvh_node *node, t_intersections *xs, 
 * t_ray *ray, t_object **objects)
 * @brief Records standard intersections for objects inside a leaf node.
 * @details Iterates through all objects owned by the leaf and populates the 
 * global intersections array.
 * @param node    The leaf node containing the objects.
 * @param xs      The global intersections container.
 * @param ray     The primary ray being cast.
 * @param objects The master array of world objects.
 */
static void	intersect_leaf(t_bvh_node *node, t_intersections *xs, t_ray *ray,
	t_object **objects)
{
	int	i;

	i = -1;
	while (++i < node->total_objects && xs->count < MAX_INTERSECTIONS)
		intersect_object(xs, ray, objects[node->first_object + i]);
}

/**
 * @fn void intersect_bvh(t_bvh *bvh, t_intersections *xs, t_ray *ray, 
 * t_object **objects)
 * @brief Tests a ray against the BVH to find all object intersections.
 * @details Uses an iterative stack approach (bypassing recursion) to traverse 
 * the tree. If the ray hits a leaf node's bounds, it tests the objects inside.
 * @param bvh     Pointer to the BVH tree.
 * @param xs      Pointer to the intersections container to populate.
 * @param ray     The ray being cast into the scene.
 * @param objects The flattened array of world objects.
 */
void	intersect_bvh(t_bvh *bvh, t_intersections *xs, t_ray *ray,
	t_object **objects)
{
	int			nodes_to_check[BVH_MAX_DEPTH];
	int			node_count;
	t_bvh_node	*current;

	if (!bvh || bvh->node_count == 0)
		return ;
	node_count = 0;
	nodes_to_check[node_count++] = 0;
	while (node_count > 0)
	{
		current = &bvh->nodes[nodes_to_check[--node_count]];
		if (!intersect_bounds(ray, &current->bounds))
			continue ;
		if (current->total_objects > 0)
		{
			intersect_leaf(current, xs, ray, objects);
			continue ;
		}
		if (node_count >= BVH_MAX_DEPTH - 2)
			continue ;
		nodes_to_check[node_count++] = current->left_index;
		nodes_to_check[node_count++] = current->right_index;
	}
}
