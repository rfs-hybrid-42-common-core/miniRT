/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_obj_faces.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 22:47:21 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 03:57:57 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"
#include "parser.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stdlib.h>
#include <stddef.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void validate_face(t_obj_parser *parser, 
 * t_obj_face_index *face_indexes)
 * @brief Ensures a decoded face request physically exists.
 * @details Validates that the requested index limits do not exceed the 
 * boundaries established during the initial counting phase. Checks for 
 * degenerate triangles (where two points use the same vertex). Enforces 
 * strict formatting uniformity within the face (e.g., if one vertex 
 * provides a normal, they all must).
 * @param parser       Pointer to the OBJ parsing state.
 * @param face_indexes The decoded array of index combinations.
 */
static void	validate_face(t_obj_parser *parser, t_obj_face_index *face_indexes)
{
	if (face_indexes[0].v <= 0 || face_indexes[0].v >= parser->v_index
		|| face_indexes[1].v <= 0 || face_indexes[1].v >= parser->v_index
		|| face_indexes[2].v <= 0 || face_indexes[2].v >= parser->v_index)
		abort_obj_parse(parser, "Face references invalid/undeclared vertex",
			NULL, OBJ_PARSE);
	if (face_indexes[0].v == face_indexes[1].v
		|| face_indexes[1].v == face_indexes[2].v
		|| face_indexes[0].v == face_indexes[2].v)
		abort_obj_parse(parser, "Degenerate triangle (duplicate vertices)",
			NULL, OBJ_PARSE);
	if ((face_indexes[0].vt > 0) != (face_indexes[1].vt > 0)
		|| (face_indexes[0].vt > 0) != (face_indexes[2].vt > 0)
		|| (face_indexes[0].vn > 0) != (face_indexes[1].vn > 0)
		|| (face_indexes[0].vn > 0) != (face_indexes[2].vn > 0))
		abort_obj_parse(parser, "Mixed vertex formatting in a single face",
			NULL, OBJ_PARSE);
}

/**
 * @fn static void extract_indexes(const char *ptr, 
 * t_obj_face_index *face_index)
 * @brief Decodes the complex vertex formatting string.
 * @details Deciphers the slash-separated blocks format standard to OBJs 
 * (`v/vt/vn` or `v//vn`). Safely isolates missing data (like missing UVs) by 
 * tracking sequential slashes.
 * @param ptr        String pointer at the start of the index block.
 * @param face_index The struct receiving the decoded integers.
 */
static void	extract_indexes(const char *ptr, t_obj_face_index *face_index)
{
	int	total_slashes;
	int	i;

	face_index->v = -1;
	total_slashes = 0;
	i = -1;
	while (ptr[++i] && !ft_isspace(ptr[i]))
	{
		if (ptr[i] == '/')
			total_slashes++;
		if ((!ft_isdigit(ptr[i]) && ptr[i] != '/') || total_slashes > 2
			|| (ptr[i] == '/' && (!ptr[i + 1] || ft_isspace(ptr[i + 1]))))
			return ;
	}
	face_index->v = ft_atoi(ptr);
	face_index->vt = 0;
	face_index->vn = 0;
	while (*ptr && ft_isdigit(*ptr))
		ptr++;
	if (*ptr == '/' && *(++ptr) != '/')
		face_index->vt = ft_atoi(ptr);
	while (*ptr && ft_isdigit(*ptr))
		ptr++;
	if (*ptr == '/')
		face_index->vn = ft_atoi(ptr + 1);
}

/**
 * @fn static void finalize_triangle(t_obj_parser *parser, 
 * t_obj_face_index *face_indexes, t_tuple *p, t_tuple *uv)
 * @brief Selects the correct triangle primitive based on available data.
 * @details If valid normal indices (`vn`) were successfully extracted 
 * from the face string, it pulls those explicit normals and constructs 
 * a Smooth Triangle (enabling Phong interpolation). Otherwise, it 
 * constructs a flat Triangle.
 * @param parser       Pointer to the OBJ parsing state.
 * @param face_indexes Array of the decoded indices.
 * @param p            Array of the 3 fetched coordinate vertices.
 * @param uv           Array of the 3 fetched texture coordinates.
 */
static void	finalize_triangle(t_obj_parser *parser,
	t_obj_face_index *face_indexes, t_tuple *p, t_tuple *uv)
{
	t_tuple		n[3];
	t_object	*triangle;

	if (face_indexes[0].vn > 0 && face_indexes[0].vn < parser->vn_index
		&& face_indexes[1].vn > 0 && face_indexes[1].vn < parser->vn_index
		&& face_indexes[2].vn > 0 && face_indexes[2].vn < parser->vn_index)
	{
		n[0] = parser->vn_array[face_indexes[0].vn];
		n[1] = parser->vn_array[face_indexes[1].vn];
		n[2] = parser->vn_array[face_indexes[2].vn];
		triangle = create_smooth_triangle(p, n, uv);
	}
	else
		triangle = create_triangle(p, uv);
	if (!triangle)
		abort_obj_parse(parser, "Memory allocation failed for triangle", NULL,
			MALLOC);
	add_child(parser->current_group, triangle);
}

/**
 * @fn static void build_triangle(t_obj_parser *parser, 
 * t_obj_face_index *face_indexes)
 * @brief O(1) Instantaneous geometric fetching.
 * @details Uses the validated integers to reach directly into the massive 
 * flat memory arrays, retrieving the exact coordinates necessary without 
 * traversing a linked list.
 * @param parser       Pointer to the OBJ parsing state.
 * @param face_indexes Array containing the 3 validated indices.
 */
static void	build_triangle(t_obj_parser *parser, t_obj_face_index *face_indexes)
{
	t_tuple		p[3];
	t_tuple		uv[3];
	int			i;

	i = -1;
	while (++i < 3)
	{
		p[i] = parser->v_array[face_indexes[i].v];
		uv[i] = vector(0, 0, 0);
		if (face_indexes[i].vt > 0 && face_indexes[i].vt < parser->vt_index)
			uv[i] = parser->vt_array[face_indexes[i].vt];
	}
	finalize_triangle(parser, face_indexes, p, uv);
}

/**
 * @fn void parse_f(char *line, t_obj_parser *parser)
 * @brief Constructs face geometry, implementing an N-Gon Triangle Fan.
 * @details High-level 3D modeling software frequently exports geometry as 
 * Quads (4 points) or N-Gons. This loop treats the very first point of the 
 * line as an anchor pivot. It then sequentially links it to the next two 
 * points, shifting forward one vertex at a time 
 * (`face_indexes[1] = face_indexes[2]`). This mathematically "fans" out, 
 * automatically slicing complex polygons into perfectly valid triangles 
 * suitable for the engine.
 * @param line   The string line starting with 'f '.
 * @param parser Pointer to the OBJ parsing state.
 */
void	parse_f(char *line, t_obj_parser *parser)
{
	const char			*ptr;
	t_obj_face_index	face_indexes[3];

	ptr = line + 1;
	while (ft_isspace(*ptr))
		ptr++;
	extract_indexes(ptr, &face_indexes[0]);
	skip_token(&ptr);
	extract_indexes(ptr, &face_indexes[1]);
	skip_token(&ptr);
	if (!*ptr || *ptr == '\r' || *ptr == '\n' || *ptr == '#')
		abort_obj_parse(parser, "Face has fewer than 3 vertices", NULL,
			OBJ_PARSE);
	while (*ptr && *ptr != '\r' && *ptr != '\n')
	{
		if (*ptr == '#')
			break ;
		extract_indexes(ptr, &face_indexes[2]);
		validate_face(parser, face_indexes);
		build_triangle(parser, face_indexes);
		face_indexes[1] = face_indexes[2];
		skip_token(&ptr);
	}
}
