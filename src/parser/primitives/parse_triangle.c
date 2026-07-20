/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_triangle.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 17:25:20 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/06 01:52:56 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"
#include "parser.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static bool is_degenerate_triangle(t_tuple *p)
 * @brief Ensures a parsed triangle possesses physical surface area.
 * @details Evaluates the cross product of the triangle's two edges. If the 
 * vertices are collinear (forming a straight line or single point), the 
 * magnitude is zero. Degenerate triangles cannot reflect light and crash the 
 * engine's normal calculators, so they must be blocked.
 * @param p Array of 3 coordinate vertices.
 * @return  True if the triangle is invalid (degenerate), false otherwise.
 */
static bool	is_degenerate_triangle(t_tuple *p)
{
	t_tuple	e1;
	t_tuple	e2;

	e1 = subtract_tuples(p[1], p[0]);
	e2 = subtract_tuples(p[2], p[0]);
	if (magnitude(cross(e1, e2)) < EPSILON)
		return (true);
	return (false);
}

/**
 * @fn static void parse_triangle_vectors(const char **ptr, bool *error, 
 * t_tuple *vecs)
 * @brief Helper to sequentially extract the 6 vectors for a flat triangle.
 * @details Pulls the 3 explicit spatial vertices (p1, p2, p3) followed by the 
 * 3 corresponding 2D UV texture mapping coordinates.
 * @param ptr   Double pointer to the raw string argument.
 * @param error Pointer to the global error flag.
 * @param vecs  Array to store the extracted tuples.
 */
static void	parse_triangle_vectors(const char **ptr, bool *error,
	t_tuple *vecs)
{
	vecs[0] = parse_tuple(ptr, error, 1.0);
	vecs[1] = parse_tuple(ptr, error, 1.0);
	vecs[2] = parse_tuple(ptr, error, 1.0);
	vecs[3] = parse_tuple(ptr, error, 0.0);
	vecs[4] = parse_tuple(ptr, error, 0.0);
	vecs[5] = parse_tuple(ptr, error, 0.0);
}

/**
 * @fn void parse_triangle(const char *ptr, t_rt_parser *parser)
 * @brief Extracts configuration data for a standard Triangle.
 * @details Validates non-collinear vertices, assigns the color, and appends it 
 * directly to the scene list. The constructor natively precomputes the face 
 * normal and edge vectors.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_triangle(const char *ptr, t_rt_parser *parser)
{
	bool		error;
	t_tuple		vecs[6];
	t_color		rgb;
	t_object	*triangle;

	error = false;
	parse_triangle_vectors(&ptr, &error, vecs);
	rgb = parse_color(&ptr, &error);
	if (error || is_degenerate_triangle(vecs))
		abort_rt_parse(parser, "Invalid or degenerate Triangle (tr)", RT_PARSE);
	triangle = create_triangle(vecs, &vecs[3]);
	if (!triangle)
		abort_rt_parse(parser, "Malloc failed for Triangle (tr) object",
			MALLOC);
	triangle->material.color = rgb;
	append_object(triangle, parser);
	parse_object_tags(triangle, ptr, parser);
}

/**
 * @fn static void parse_smooth_triangle_vectors(const char **ptr, bool *error, 
 * t_tuple *vecs)
 * @brief Helper to sequentially extract the 9 vectors for a smooth triangle.
 * @details Pulls the 3 spatial vertices, followed by 3 explicit vertex 
 * normals, followed finally by the 3 UV mapping coordinates.
 * @param ptr   Double pointer to the raw string argument.
 * @param error Pointer to the global error flag.
 * @param vecs  Array to store the extracted tuples.
 */
static void	parse_smooth_triangle_vectors(const char **ptr, bool *error,
	t_tuple *vecs)
{
	vecs[0] = parse_tuple(ptr, error, 1.0);
	vecs[1] = parse_tuple(ptr, error, 1.0);
	vecs[2] = parse_tuple(ptr, error, 1.0);
	vecs[3] = parse_normal(ptr, error);
	vecs[4] = parse_normal(ptr, error);
	vecs[5] = parse_normal(ptr, error);
	vecs[6] = parse_tuple(ptr, error, 0.0);
	vecs[7] = parse_tuple(ptr, error, 0.0);
	vecs[8] = parse_tuple(ptr, error, 0.0);
}

/**
 * @fn void parse_smooth_triangle(const char *ptr, t_rt_parser *parser)
 * @brief Extracts configuration data for an interpolated Smooth Triangle.
 * @details Loads the full vertex, normal, and UV parameter sets required for 
 * Barycentric Phong shading across the mesh surface.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_smooth_triangle(const char *ptr, t_rt_parser *parser)
{
	bool		error;
	t_tuple		vecs[9];
	t_color		rgb;
	t_object	*smooth_triangle;

	error = false;
	parse_smooth_triangle_vectors(&ptr, &error, vecs);
	rgb = parse_color(&ptr, &error);
	if (error || is_degenerate_triangle(vecs))
		abort_rt_parse(parser, "Invalid or degenerate Smooth Triangle (smt)",
			RT_PARSE);
	smooth_triangle = create_smooth_triangle(vecs, &vecs[3], &vecs[6]);
	if (!smooth_triangle)
		abort_rt_parse(parser,
			"Malloc failed for Smooth Triangle (smt) object", MALLOC);
	smooth_triangle->material.color = rgb;
	append_object(smooth_triangle, parser);
	parse_object_tags(smooth_triangle, ptr, parser);
}
