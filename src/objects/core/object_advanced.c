/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   object_advanced.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:05:01 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/03 15:17:11 by maaugust         ###   ########.fr       */
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
 * @fn t_object *create_torus(double major_r, double minor_r)
 * @brief Allocates and initializes a Torus primitive.
 * @details Spawns a mathematical donut shape resting flat on the XZ plane. 
 * @param major_r Defines the ring's distance from the center to the tube.
 * @param minor_r Defines the thickness/radius of the tube itself.
 * @return Pointer to the torus, or NULL on memory failure.
 */
t_object	*create_torus(double major_r, double minor_r)
{
	t_object	*object;

	object = malloc(sizeof(t_object));
	if (!object)
		return (NULL);
	init_object_defaults(object);
	object->type = TORUS;
	object->data.torus = (t_torus){major_r, minor_r};
	return (object);
}

/**
 * @fn t_object *create_triangle(t_tuple *p, t_tuple *uv)
 * @brief Allocates and initializes a standard Triangle primitive.
 * @details Precomputes the edge vectors (e[0], e[1]) and the geometric face 
 * normal. This perfectly sets up the triangle for the highly optimized 
 * Möller–Trumbore intersection algorithm.
 * @param p  Array of 3 tuples representing the vertices.
 * @param uv Array of 3 tuples representing UV mapping coordinates.
 * @return   Pointer to the triangle.
 */
t_object	*create_triangle(t_tuple *p, t_tuple *uv)
{
	t_object	*object;

	object = malloc(sizeof(t_object));
	if (!object)
		return (NULL);
	init_object_defaults(object);
	object->type = TRIANGLE;
	object->data.triangle.p[0] = p[0];
	object->data.triangle.p[1] = p[1];
	object->data.triangle.p[2] = p[2];
	object->data.triangle.e[0] = subtract_tuples(p[1], p[0]);
	object->data.triangle.e[1] = subtract_tuples(p[2], p[0]);
	object->data.triangle.uv[0] = uv[0];
	object->data.triangle.uv[1] = uv[1];
	object->data.triangle.uv[2] = uv[2];
	object->data.triangle.normal = normalize(cross(object->data.triangle.e[1],
				object->data.triangle.e[0]));
	return (object);
}

/**
 * @fn t_object *create_smooth_triangle(t_tuple *p, t_tuple *n, t_tuple *uv)
 * @brief Allocates and initializes a Smooth Triangle primitive.
 * @details Stores explicit vertex normals alongside the spatial vertices. This 
 * allows for Phong interpolation across the face, drastically improving the 
 * look of low-poly parsed OBJ meshes.
 * @param p  Array of 3 tuples representing the absolute vertices.
 * @param n  Array of 3 tuples representing the true vertex normals.
 * @param uv Array of 3 tuples representing 2D texture mapping coordinates.
 * @return   Pointer to the smooth triangle, or NULL on memory failure.
 */
t_object	*create_smooth_triangle(t_tuple *p, t_tuple *n, t_tuple *uv)
{
	t_object	*object;

	object = malloc(sizeof(t_object));
	if (!object)
		return (NULL);
	init_object_defaults(object);
	object->type = SMOOTH_TRIANGLE;
	object->data.smooth_triangle.p[0] = p[0];
	object->data.smooth_triangle.p[1] = p[1];
	object->data.smooth_triangle.p[2] = p[2];
	object->data.smooth_triangle.e[0] = subtract_tuples(p[1], p[0]);
	object->data.smooth_triangle.e[1] = subtract_tuples(p[2], p[0]);
	object->data.smooth_triangle.n[0] = n[0];
	object->data.smooth_triangle.n[1] = n[1];
	object->data.smooth_triangle.n[2] = n[2];
	object->data.smooth_triangle.uv[0] = uv[0];
	object->data.smooth_triangle.uv[1] = uv[1];
	object->data.smooth_triangle.uv[2] = uv[2];
	return (object);
}

/**
 * @fn t_object *create_group(void)
 * @brief Allocates and initializes a Group logical node.
 * @details Spawns an empty container used to link multiple objects together 
 * under a single transformation hierarchy. The bounding box is initialized to 
 * negative infinity, ready to physically expand as children are added.
 * @return Pointer to the group, or NULL on memory failure.
 */
t_object	*create_group(void)
{
	t_object	*object;

	object = malloc(sizeof(t_object));
	if (!object)
		return (NULL);
	init_object_defaults(object);
	object->type = GROUP;
	object->data.group = (t_group){empty_bounding_box(), NULL};
	return (object);
}

/**
 * @fn t_object *create_csg(t_csg_op operation, t_object *left, t_object *right)
 * @brief Allocates a Constructive Solid Geometry logical node.
 * @details Encapsulates a boolean operation (Union, Intersection, Difference) 
 * to combine two sub-shapes into complex geometry.
 * @param operation The mathematical boolean operation to perform.
 * @param left      Pointer to the primary/left shape.
 * @param right     Pointer to the secondary/right shape.
 * @return          Pointer to the CSG node.
 */
t_object	*create_csg(t_csg_op operation, t_object *left, t_object *right)
{
	t_object	*object;

	object = malloc(sizeof(t_object));
	if (!object)
		return (NULL);
	init_object_defaults(object);
	object->type = CSG;
	object->data.csg = (t_csg){operation, left, right, NULL, NULL};
	return (object);
}
