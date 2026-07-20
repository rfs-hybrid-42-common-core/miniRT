/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bounds_operations.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 18:36:06 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 14:06:34 by maaugust         ###   ########.fr       */
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
 * @fn t_bounds empty_bounding_box(void)
 * @brief Initializes an inverted, invalid bounding box.
 * @details By setting min to +Infinity and max to -Infinity, any point added 
 * to this box will instantly override the limits and set the true baseline.
 * @return The empty bounding box.
 */
t_bounds	empty_bounding_box(void)
{
	return ((t_bounds){
		point(INFINITY, INFINITY, INFINITY),
		point(-INFINITY, -INFINITY, -INFINITY)
	});
}

/**
 * @fn void add_point_to_bounds(t_bounds *box, t_tuple point)
 * @brief Expands a bounding box to include a 3D coordinate.
 * @details Compares the point's X, Y, and Z values against the current minimum 
 * and maximum extents of the box, pushing the boundaries outward if necessary.
 * @param box   The box to expand.
 * @param point The point to encompass.
 */
void	add_point_to_bounds(t_bounds *box, t_tuple point)
{
	box->min.x = get_fmin(box->min.x, point.x);
	box->min.y = get_fmin(box->min.y, point.y);
	box->min.z = get_fmin(box->min.z, point.z);
	box->max.x = get_fmax(box->max.x, point.x);
	box->max.y = get_fmax(box->max.y, point.y);
	box->max.z = get_fmax(box->max.z, point.z);
}

/**
 * @fn void add_box_to_bounds(t_bounds *box1, t_bounds *box2)
 * @brief Merges two bounding boxes together (Union).
 * @details Effectively takes the minimum and maximum extremes of both bounding 
 * boxes to create a single master box that completely envelopes both volumes.
 * @param box1 The base box to expand.
 * @param box2 The box being enveloped.
 */
void	add_box_to_bounds(t_bounds *box1, t_bounds *box2)
{
	add_point_to_bounds(box1, box2->min);
	add_point_to_bounds(box1, box2->max);
}

/**
 * @fn t_bounds parent_space_bounds_of(t_object *object)
 * @brief Calculates a bounding box in parent/world space.
 * @details Retrieves the local bounds and transforms them by the object's 
 * transformation matrix. Critically, if the object is in motion (motion blur), 
 * it merges the start and end bounding boxes to guarantee the BVH encapsulates 
 * the object across all frames of time.
 * @param object The object to bound.
 * @return       The transformed, motion-safe bounding box.
 */
t_bounds	parent_space_bounds_of(t_object *object)
{
	t_bounds	bounds;
	t_bounds	box_start;
	t_bounds	box_end;

	bounds = bounds_of(object);
	box_start = transform_bounds(&bounds, &object->transform);
	if (object->is_moving)
	{
		box_end = transform_bounds(&bounds, &object->transform_end);
		add_box_to_bounds(&box_start, &box_end);
	}
	return (box_start);
}
