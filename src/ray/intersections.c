/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersections.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 23:12:03 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:01:01 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "ray.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stddef.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void route_intersection(t_intersections *xs, t_ray *ray, 
 * t_object *object)
 * @brief Routes the intersection calculation to the correct shape function.
 * @details Utilizes a static array of function pointers (jump table) mapped to 
 * the `t_object_type` enum to eliminate large if-else branching logic.
 * @param xs     Pointer to the intersections container.
 * @param ray    The transformed local-space ray.
 * @param object The object being intersected.
 */
static void	route_intersection(t_intersections *xs, t_ray *ray,
	t_object *object)
{
	static void	(*intersect[])(t_intersections *, t_ray *, t_object *) = {
	[SPHERE] = intersect_sphere,
	[PLANE] = intersect_plane,
	[CUBE] = intersect_cube,
	[CYLINDER] = intersect_cylinder,
	[CONE] = intersect_cone,
	[TORUS] = intersect_torus,
	[TRIANGLE] = intersect_triangle,
	[SMOOTH_TRIANGLE] = intersect_triangle,
	[GROUP] = intersect_group,
	[CSG] = intersect_csg
	};

	if (object->type < SPHERE || object->type >= OBJECT_COUNT
		|| !intersect[object->type])
		return ;
	intersect[object->type](xs, ray, object);
}

/**
 * @fn void intersect_object(t_intersections *xs, t_ray *ray, t_object *object)
 * @brief The main entry point for calculating ray-object intersections.
 * @details Converts the world-space ray into the object's local space using 
 * its inverse transform. Handles temporal motion blur by interpolating the 
 * matrix if the object is currently moving.
 * @param xs     Pointer to the intersections container.
 * @param ray    The world-space ray.
 * @param object The target object.
 */
void	intersect_object(t_intersections *xs, t_ray *ray, t_object *object)
{
	t_matrix	interpolated_transform;
	t_matrix	interpolated_inverse;
	t_ray		transformed_ray;

	if (xs->count >= MAX_INTERSECTIONS)
		return ;
	if (!object->is_moving)
		interpolated_inverse = object->transform_inverse;
	else
	{
		interpolated_transform = interpolate_transform(&object->transform,
				&object->transform_end, ray->time);
		interpolated_inverse = inverse_matrix(&interpolated_transform);
	}
	transform_ray(&transformed_ray, ray, &interpolated_inverse);
	route_intersection(xs, &transformed_ray, object);
}

/**
 * @fn void add_intersection(t_intersections *xs, double t, t_object *object)
 * @brief Safely registers a calculated intersection.
 * @details Appends the distance and object pointer to the tracking array, 
 * guarding against memory corruption if the maximum capacity is reached.
 * @param xs     Pointer to the intersections container.
 * @param t      The distance of the hit.
 * @param object The object that was hit.
 */
void	add_intersection(t_intersections *xs, double t, t_object *object)
{
	if (xs->count >= MAX_INTERSECTIONS)
		return ;
	xs->array[xs->count].t = t;
	xs->array[xs->count].object = object;
	xs->array[xs->count].u = 0.0;
	xs->array[xs->count].v = 0.0;
	xs->count++;
}

/**
 * @fn t_intersection *hit(t_intersections *xs)
 * @brief Identifies the visibly closest intersection point.
 * @details Scans the intersection array and returns the lowest positive `t` 
 * value. Negative `t` values (objects behind the camera) are ignored.
 * @param xs Pointer to the populated intersections container.
 * @return   Pointer to the closest valid intersection, or NULL if none exist.
 */
t_intersection	*hit(t_intersections *xs)
{
	t_intersection	*closest;
	int				i;

	closest = NULL;
	i = -1;
	while (++i < xs->count)
		if (xs->array[i].t > EPSILON
			&& (!closest || xs->array[i].t < closest->t))
			closest = &xs->array[i];
	return (closest);
}

/**
 * @fn void sort_intersections(t_intersections *xs)
 * @brief Sorts the intersections array in ascending order based on distance.
 * @details Uses an Insertion Sort algorithm. Sorting is mandatory for complex 
 * overlapping geometry like CSG filtering and accurate glass refraction 
 * tracking.
 * @param xs Pointer to the intersections container to be sorted.
 */
void	sort_intersections(t_intersections *xs)
{
	t_intersection	tmp;
	int				i;
	int				j;

	i = 0;
	while (++i < xs->count)
	{
		tmp = xs->array[i];
		j = i - 1;
		while (j >= 0 && xs->array[j].t > tmp.t + EPSILON)
		{
			xs->array[j + 1] = xs->array[j];
			j--;
		}
		xs->array[j + 1] = tmp;
	}
}
