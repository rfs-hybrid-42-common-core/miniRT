/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   csg.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 22:49:54 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:25:19 by maaugust         ###   ########.fr       */
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
 * @fn static bool is_included(t_object *node, t_object *target)
 * @brief Checks if a specific object belongs to a logical tree.
 * @details Recursively transverses Groups and CSG nodes to determine if the 
 * target shape is housed anywhere within the specified left or right branch. 
 * Essential for resolving boolean math intersections.
 * @param node   The root node to search within.
 * @param target The specific geometry being searched for.
 * @return       True if the target is found inside the node, false otherwise.
 */
static bool	is_included(t_object *node, t_object *target)
{
	t_list		*current;
	t_object	*child;

	if (node == target)
		return (true);
	if (node->type == GROUP)
	{
		current = node->data.group.children;
		while (current)
		{
			child = (t_object *)current->content;
			if (is_included(child, target))
				return (true);
			current = current->next;
		}
	}
	if (node->type == CSG)
		return (is_included(node->data.csg.left, target)
			|| is_included(node->data.csg.right, target));
	return (false);
}

/**
 * @fn static bool intersection_allowed(t_csg_op operation, bool lhit, 
 * bool inl, bool inr)
 * @brief CSG state machine evaluating boolean operations.
 * @details Uses truth tables to determine if a specific intersection boundary 
 * should be preserved or discarded based on whether the ray is currently 
 * inside or outside the opposing shape.
 * @param operation The CSG operation (Union, Intersect, Difference).
 * @param lhit      True if the current intersection belongs to the Left shape.
 * @param inl       True if the ray is currently inside the Left shape.
 * @param inr       True if the ray is currently inside the Right shape.
 * @return          True if the intersection point is geometrically valid.
 */
static bool	intersection_allowed(t_csg_op operation, bool lhit, bool inl,
	bool inr)
{
	if (operation == CSG_UNION)
		return ((lhit && !inr) || (!lhit && !inl));
	if (operation == CSG_INTERSECTION)
		return ((lhit && inr) || (!lhit && inl));
	if (operation == CSG_DIFFERENCE)
		return ((lhit && !inr) || (!lhit && inl));
	return (false);
}

/**
 * @fn static void filter_intersections(t_intersections *xs, t_object *csg)
 * @brief Filters raw intersections to enforce CSG boolean logic.
 * @details Iterates through the chronologically sorted hits of the left and 
 * right shapes. Tracks the ray's interior/exterior state and discards hits 
 * that fall inside visually excluded zones.
 * @param xs  Pointer to the array of raw, sorted intersections.
 * @param csg The CSG logical node enforcing the rules.
 */
static void	filter_intersections(t_intersections *xs, t_object *csg)
{
	bool	lhit;
	bool	inl;
	bool	inr;
	int		result;
	int		i;

	inl = false;
	inr = false;
	result = 0;
	i = -1;
	while (++i < xs->count)
	{
		lhit = is_included(csg->data.csg.left, xs->array[i].object);
		if (intersection_allowed(csg->data.csg.operation, lhit, inl, inr))
			xs->array[result++] = xs->array[i];
		if (lhit)
			inl = !inl;
		else
			inr = !inr;
	}
	xs->count = result;
}

/**
 * @fn void intersect_csg(t_intersections *xs, t_ray *ray, t_object *object)
 * @brief Master intersection calculator for Constructive Solid Geometry.
 * @details Collects hits from both the left and right shapes into an isolated 
 * local container, sorts them by distance, applies the boolean state machine 
 * filter, and finally appends only the valid hits back to the main container.
 * @param xs     Pointer to the global intersections container.
 * @param ray    The transformed local-space ray.
 * @param object The CSG node being tested.
 */
void	intersect_csg(t_intersections *xs, t_ray *ray, t_object *object)
{
	t_intersections	xs_csg;
	int				i;

	xs_csg.count = 0;
	intersect_object(&xs_csg, ray, object->data.csg.left);
	intersect_object(&xs_csg, ray, object->data.csg.right);
	sort_intersections(&xs_csg);
	filter_intersections(&xs_csg, object);
	i = -1;
	while (++i < xs_csg.count)
		if (xs->count < MAX_INTERSECTIONS)
			xs->array[xs->count++] = xs_csg.array[i];
}
