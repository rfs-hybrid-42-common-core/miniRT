/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   object_basic.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 05:57:42 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/15 14:30:54 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "objects.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stddef.h>
#include <stdlib.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn t_object *create_sphere(void)
 * @brief Allocates and initializes a Sphere primitive.
 * @details Spawns a mathematical unit sphere at the origin (0, 0, 0) with a 
 * radius of 1.0. All subsequent size and position adjustments are handled via 
 * its transformation matrix.
 * @return Pointer to the sphere, or NULL on memory failure.
 */
t_object	*create_sphere(void)
{
	t_object	*object;

	object = malloc(sizeof(t_object));
	if (!object)
		return (NULL);
	init_object_defaults(object);
	object->type = SPHERE;
	object->data.sphere = (t_sphere){point(0, 0, 0), 1.0};
	return (object);
}

/**
 * @fn t_object *create_plane(void)
 * @brief Allocates and initializes a Plane primitive.
 * @details Spawns a mathematical plane stretching infinitely across the XZ 
 * axes. Its base geometric normal points straight up (0, 1, 0) by default 
 * until rotated.
 * @return Pointer to the plane, or NULL on memory failure.
 */
t_object	*create_plane(void)
{
	t_object	*object;

	object = malloc(sizeof(t_object));
	if (!object)
		return (NULL);
	init_object_defaults(object);
	object->type = PLANE;
	return (object);
}

/**
 * @fn t_object *create_cube(void)
 * @brief Allocates and initializes a Cube primitive.
 * @details Spawns a unit cube (AABB) centered at the origin, with sides 
 * extending exactly from -1.0 to 1.0 on all axes.
 * @return Pointer to the cube, or NULL on memory failure.
 */
t_object	*create_cube(void)
{
	t_object	*object;

	object = malloc(sizeof(t_object));
	if (!object)
		return (NULL);
	init_object_defaults(object);
	object->type = CUBE;
	return (object);
}

/**
 * @fn t_object *create_cylinder(void)
 * @brief Allocates and initializes a Cylinder primitive.
 * @details Defaults to a unit-height tube (spanning -0.5 to 0.5 on the 
 * Y axis), open at the ends. This allows standard transform matrices to 
 * scale its height and radius accurately from a normalized baseline.
 * @return Pointer to the cylinder.
 */
t_object	*create_cylinder(void)
{
	t_object	*object;

	object = malloc(sizeof(t_object));
	if (!object)
		return (NULL);
	init_object_defaults(object);
	object->type = CYLINDER;
	object->data.cylinder = (t_cylinder){-0.5, 0.5, false};
	return (object);
}

/**
 * @fn t_object *create_cone(void)
 * @brief Allocates and initializes a Cone primitive.
 * @details Defaults to a unit-height cone (spanning from -1.0 to 0.0 on the 
 * Y axis) with an open base. This ensures the tip remains securely locked at 
 * the local origin (0,0,0) while standard transform matrices scale its height 
 * and radius appropriately.
 * @return Pointer to the cone, or NULL on memory failure.
 */
t_object	*create_cone(void)
{
	t_object	*object;

	object = malloc(sizeof(t_object));
	if (!object)
		return (NULL);
	init_object_defaults(object);
	object->type = CONE;
	object->data.cone = (t_cone){-1.0, 0.0, false};
	return (object);
}
