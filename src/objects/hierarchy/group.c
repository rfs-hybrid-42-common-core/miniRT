/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   group.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 04:05:57 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:22:56 by maaugust         ###   ########.fr       */
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
 * @fn void add_child(t_object *group, t_object *object)
 * @brief Appends a new child object to a group node.
 * @details Sets the parent pointer of the child, links it to the list, and 
 * physically expands the group's bounding box to fully encompass the new child.
 * @param group  The group logical node.
 * @param object The child geometry or nested group to add.
 */
void	add_child(t_object *group, t_object *object)
{
	t_list		*child;
	t_bounds	bounds;

	child = ft_lstnew(object);
	if (!child)
		return ;
	ft_lstadd_back(&group->data.group.children, child);
	object->parent = group;
	bounds = parent_space_bounds_of(object);
	add_box_to_bounds(&group->data.group.bounds, &bounds);
}

/**
 * @fn void intersect_group(t_intersections *xs, t_ray *ray, 
 * t_object *object)
 * @brief Checks for intersections against all children inside a group.
 * @details First performs an AABB intersection against the group's bounding 
 * box. If the ray misses the box, all children inside are safely bypassed, 
 * saving significant CPU cycles.
 * @param xs     Pointer to the intersections container.
 * @param ray    The transformed local-space ray.
 * @param object The group logical node.
 */
void	intersect_group(t_intersections *xs, t_ray *ray, t_object *object)
{
	t_list	*current;

	if (!intersect_bounds(ray, &object->data.group.bounds))
		return ;
	current = object->data.group.children;
	while (current)
	{
		if (xs->count >= MAX_INTERSECTIONS)
			break ;
		intersect_object(xs, ray, (t_object *)current->content);
		current = current->next;
	}
}
