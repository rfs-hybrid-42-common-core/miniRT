/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   computations.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 15:01:59 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/02 02:24:14 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "shading.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void remove_object_from_containers(t_object **containers, 
 * int *count, int pos)
 * @brief Removes an object from the active refraction tracking array.
 * @details Shifts all subsequent array elements down by one, keeping the array 
 * contiguous. Executed when a ray fully exits a refractive object.
 * @param containers Array of active objects the ray is currently inside.
 * @param count      Pointer to the active object count.
 * @param pos        Index of the object to remove.
 */
static void	remove_object_from_containers(t_object **containers, int *count,
	int pos)
{
	int	i;

	i = pos - 1;
	while (++i < *count - 1)
		containers[i] = containers[i + 1];
	(*count)--;
}

/**
 * @fn static int find_object_in_containers(t_object **containers, int count, 
 * t_object *target)
 * @brief Searches for a specific object within the refraction containers array.
 * @details Determines if a ray is entering a new object or exiting one it is 
 * already inside of.
 * @param containers Array of active objects.
 * @param count      Number of active objects.
 * @param target     The object being searched for.
 * @return           The index of the object if found, -1 otherwise.
 */
static int	find_object_in_containers(t_object **containers, int count,
	t_object *target)
{
	int	i;

	i = -1;
	while (++i < count)
		if (containers[i] == target)
			return (i);
	return (-1);
}

/**
 * @fn static void update_refraction(t_computations *comps, 
 * t_object **containers, int count, bool is_n1)
 * @brief Sets the n1 (exiting) or n2 (entering) refractive indices.
 * @details If the container array is empty, it means the ray is in the vacuum 
 * of space, which natively has a refractive index of 1.0. Otherwise, it pulls 
 * the index from the topmost object in the container stack.
 * @param comps      The computation struct to update.
 * @param containers The active refractive objects array.
 * @param count      The current depth of the containers.
 * @param is_n1      True to set n1, false to set n2.
 */
static void	update_refraction(t_computations *comps,
	t_object **containers, int count, bool is_n1)
{
	if (is_n1)
	{
		if (count == 0)
			comps->n1 = 1.0;
		else
			comps->n1 = containers[count - 1]->material.refractive_index;
	}
	else
	{
		if (count == 0)
			comps->n2 = 1.0;
		else
			comps->n2 = containers[count - 1]->material.refractive_index;
	}
}

/**
 * @fn static void calculate_refraction(t_computations *comps, 
 * t_intersection *closest, t_intersections *xs)
 * @brief Simulates the ray's traversal through all objects up to the hit point.
 * @details By iterating through every intersection *before* the visible hit, 
 * this function correctly builds a state array of what transparent objects the 
 * ray is currently inside. This accurately assigns the 'n1' and 'n2' values 
 * required by Snell's Law to bend the light.
 * @param comps   The computations struct being prepared.
 * @param closest The specific visible hit being rendered.
 * @param xs      The full list of all intersections along the ray path.
 */
static void	calculate_refraction(t_computations *comps, t_intersection *closest,
			t_intersections *xs)
{
	t_object	*containers[MAX_CONTAINERS];
	int			count;
	int			pos;
	int			i;

	count = 0;
	i = -1;
	while (++i < xs->count)
	{
		if (fabs(xs->array[i].t - closest->t) < EPSILON
			&& xs->array[i].object == closest->object)
			update_refraction(comps, containers, count, true);
		pos = find_object_in_containers(containers, count, xs->array[i].object);
		if (pos >= 0)
			remove_object_from_containers(containers, &count, pos);
		else if (count < MAX_CONTAINERS)
			containers[count++] = xs->array[i].object;
		if (fabs(xs->array[i].t - closest->t) < EPSILON
			&& xs->array[i].object == closest->object)
		{
			update_refraction(comps, containers, count, false);
			break ;
		}
	}
}

/**
 * @fn t_computations prepare_computations(t_intersection *hit, t_ray *ray, 
 * t_intersections *xs)
 * @brief Master initialization function for surface shading data.
 * @details Precomputes vectors, handles the 'inside' inversion logic for 
 * internal reflections, applies Tangent Space normal mapping perturbation, 
 * and offsets the interaction points by EPSILON to prevent mathematical 
 * shadow acne.
 * @param hit Pointer to the closest visible intersection.
 * @param ray The viewing ray.
 * @param xs  The full list of all ray intersections.
 * @return    A populated computations struct ready for the shading pipeline.
 */
t_computations	prepare_computations(t_intersection *hit, t_ray *ray,
	t_intersections *xs)
{
	t_computations	comps;
	t_tuple			geometric_normal;

	comps.t = hit->t;
	comps.object = hit->object;
	comps.time = ray->time;
	comps.point = ray_position(ray, comps.t);
	comps.eyev = negating_tuple(ray->direction);
	comps.normalv = normal_at(hit, comps.point, comps.time);
	comps.inside = dot(comps.normalv, comps.eyev) < -EPSILON;
	if (comps.inside)
		comps.normalv = negating_tuple(comps.normalv);
	comps.over_point = add_tuples(comps.point,
			scaling_tuple(comps.normalv, SURFACE_BIAS));
	comps.under_point = subtract_tuples(comps.point,
			scaling_tuple(comps.normalv, SURFACE_BIAS));
	geometric_normal = comps.normalv;
	comps.normalv = perturb_normal(&comps);
	if (dot(comps.normalv, geometric_normal) < 0.0)
		comps.normalv = geometric_normal;
	comps.reflectv = reflect(ray->direction, comps.normalv);
	return (calculate_refraction(&comps, hit, xs), comps);
}
