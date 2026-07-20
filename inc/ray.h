/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 22:49:45 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:20:06 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAY_H
# define RAY_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "math_rt.h"
# include "objects.h"

/* ========================================================================== */
/* MACROS                                                                     */
/* ========================================================================== */

/**
 * @def MAX_INTERSECTIONS
 * @brief Maximum number of intersections tracked per ray.
 * @details Uses a statically allocated array instead of malloc/realloc to 
 * guarantee lightning-fast performance during the render loop. 128 is high 
 * enough to handle complex overlapping CSG groups and BVH traversals.
 */
# ifndef MAX_INTERSECTIONS
#  define MAX_INTERSECTIONS	128
# endif

/* ========================================================================== */
/* TYPEDEFS & DATA STRUCTURES                                                 */
/* ========================================================================== */

/**
 * @typedef t_object
 * @brief Forward declaration of the geometric object structure.
 * @details Resolves circular dependency issues between the ray and object 
 * headers. Represents any renderable primitive, group, or CSG node in the 
 * scene.
 */
typedef struct s_object	t_object;

/**
 * @struct s_ray
 * @brief Represents a mathematical ray cast into the scene.
 * @details Contains the origin, direction, and precomputed values required 
 * for optimized AABB intersection tests and temporal motion blur.
 * @var origin            The starting point of the ray.
 * @var direction         The normalized vector indicating the ray's path.
 * @var direction_inverse Precomputed 1.0 / direction. Used to turn expensive 
 * divisions into fast multiplications during BVH Slab Method bounding tests.
 * @var time              The temporal parameter (0.0 to 1.0) for motion blur.
 */
typedef struct s_ray
{
	t_tuple	origin;
	t_tuple	direction;
	t_tuple	direction_inverse;
	double	time;
}	t_ray;

/**
 * @struct s_intersection
 * @brief Records a single point where a ray intersects an object.
 * @details Stores the exact distance `t` and a pointer to the hit object. 
 * Also holds Barycentric coordinates (`u` and `v`) specifically for smooth 
 * triangle normal interpolation.
 * @var u      The U Barycentric coordinate (used by Möller–Trumbore).
 * @var v      The V Barycentric coordinate (used by Möller–Trumbore).
 * @var t      The distance from the ray origin to the intersection point.
 * @var object A pointer to the specific object that was hit.
 */
typedef struct s_intersection
{
	double		u;
	double		v;
	double		t;
	t_object	*object;
}	t_intersection;

/**
 * @struct s_intersections
 * @brief A lightweight container for tracking multiple ray hits.
 * @details Pre-allocated array used to collect all intersection points along 
 * a single ray's path before sorting them to find the closest visible hit.
 * @var count The current number of recorded intersections.
 * @var array The fixed-size array holding the intersection data.
 */
typedef struct s_intersections
{
	int				count;
	t_intersection	array[MAX_INTERSECTIONS];
}	t_intersections;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ----------------------------- Ray Management ----------------------------- */
t_ray			create_ray(t_tuple origin, t_tuple direction, double time);
t_tuple			ray_position(t_ray *ray, double t);
void			transform_ray(t_ray *out, t_ray *in, t_matrix *matrix);

/* ------------------------ Intersection Processing ------------------------- */
void			intersect_object(t_intersections *xs, t_ray *ray,
					t_object *object);
void			add_intersection(t_intersections *xs, double t,
					t_object *object);
t_intersection	*hit(t_intersections *xs);
void			sort_intersections(t_intersections *xs);

#endif
