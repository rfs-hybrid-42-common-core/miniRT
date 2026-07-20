/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_partition.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 15:24:02 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/02 15:39:09 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "bvh.h"
#include "math_rt.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static double get_axis_value(t_object *object, int axis)
 * @brief Extracts the spatial center of an object along a given axis.
 * @details Used to determine which side of the partition line an object 
 * belongs to during BVH construction. 
 * @param object Pointer to the object.
 * @param axis   0 for X, 1 for Y, 2 for Z.
 * @return       The center coordinate of the object's bounding box on that 
 * axis.
 */
static double	get_axis_value(t_object *object, int axis)
{
	t_bounds	box;
	t_tuple		center;

	box = parent_space_bounds_of(object);
	center = point(box.min.x + (box.max.x - box.min.x) / 2.0,
			box.min.y + (box.max.y - box.min.y) / 2.0,
			box.min.z + (box.max.z - box.min.z) / 2.0);
	if (axis == 0)
		return (center.x);
	if (axis == 1)
		return (center.y);
	return (center.z);
}

/**
 * @fn static void sort_objects(t_object **objects, int start, int end, 
 * int axis)
 * @brief Performs an in-place Quicksort on the object array.
 * @details Sorts the geometric primitives mathematically along the specified 
 * axis using their centroids. This perfectly aligns them for the SAH sweep.
 * @param objs  The master array of objects.
 * @param start The starting index of the active segment.
 * @param end   The ending index of the active segment.
 * @param axis  The target evaluation axis (0=X, 1=Y, 2=Z).
 */
static void	sort_objects(t_object **objects, int start, int end, int axis)
{
	t_object	*tmp;
	double		pivot;
	int			i;
	int			j;

	i = start;
	j = end;
	pivot = get_axis_value(objects[(start + end) / 2], axis);
	while (i <= j)
	{
		while (get_axis_value(objects[i], axis) < pivot)
			i++;
		while (get_axis_value(objects[j], axis) > pivot)
			j--;
		if (i <= j)
		{
			tmp = objects[i];
			objects[i++] = objects[j];
			objects[j--] = tmp;
		}
	}
	if (start < j)
		sort_objects(objects, start, j, axis);
	if (i < end)
		sort_objects(objects, i, end, axis);
}

/**
 * @fn static double bounds_surface_area(t_bounds box)
 * @brief Calculates the total surface area of a bounding volume.
 * @details Core to the SAH algorithm. The probability of a random ray hitting 
 * a bounding box is directly proportional to its surface area.
 * @param box The bounding box to evaluate.
 * @return    The floating-point area of the box.
 */
static double	bounds_surface_area(t_bounds box)
{
	t_tuple	extent;

	extent = subtract_tuples(box.max, box.min);
	return (2.0 * (extent.x * extent.y + extent.x * extent.z
			+ extent.y * extent.z));
}

/**
 * @fn static double get_split_cost(t_object **objects, int start, int split, 
 * int end)
 * @brief Calculates the mathematical cost of a proposed SAH split line.
 * @details Determines the theoretical bounding boxes of the left and right 
 * sides, calculates their surface areas, and multiplies them by the number of 
 * objects contained within. Lower cost = faster render times.
 * @param objects  The array of objects.
 * @param start    The start index.
 * @param split    The evaluated split index.
 * @param end      The end index.
 * @return         The calculated SAH cost metric.
 */
static double	get_split_cost(t_object **objects, int start, int split,
	int end)
{
	t_bounds	left_box;
	t_bounds	right_box;
	t_bounds	tmp;
	int			i;

	left_box = empty_bounding_box();
	i = start - 1;
	while (++i < split)
	{
		tmp = parent_space_bounds_of(objects[i]);
		add_box_to_bounds(&left_box, &tmp);
	}
	right_box = empty_bounding_box();
	i = split - 1;
	while (++i < end)
	{
		tmp = parent_space_bounds_of(objects[i]);
		add_box_to_bounds(&right_box, &tmp);
	}
	return (bounds_surface_area(left_box) * (split - start)
		+ bounds_surface_area(right_box) * (end - split));
}

/**
 * @fn int partition_objects(t_object **objs, t_bvh_range range, int axis)
 * @brief Locates the optimal split index using the Surface Area Heuristic.
 * @details Sorts the active node's array segment. Sweeps through every single 
 * object index to find the exact boundary that minimizes empty space and groups 
 * tightly clustered geometry together.
 * @param objs  The array of objects.
 * @param range The start and end indices of the node.
 * @param axis  The longest axis (0=X, 1=Y, 2=Z) to evaluate.
 * @return      The optimal array index to divide the left and right nodes.
 */
int	partition_objects(t_object **objects, t_bvh_range range, int axis)
{
	int		best_split;
	double	best_cost;
	double	cost;
	int		stride;
	int		i;

	sort_objects(objects, range.start, range.end - 1, axis);
	best_split = range.start + 1;
	best_cost = get_split_cost(objects, range.start, best_split, range.end);
	stride = (range.end - range.start) / 8;
	if (stride < 1)
		stride = 1;
	i = range.start + 1;
	while (i < range.end)
	{
		cost = get_split_cost(objects, range.start, i, range.end);
		if (cost < best_cost)
		{
			best_cost = cost;
			best_split = i;
		}
		i += stride;
	}
	return (best_split);
}
