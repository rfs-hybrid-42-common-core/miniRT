/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_texture_tags.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 14:57:15 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 03:31:04 by maaugust         ###   ########.fr       */
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

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn t_uv_map get_uv_map(t_object *object, t_rt_parser *parser)
 * @brief Automatically assigns the correct unwrapping logic based on geometry.
 * @details Ensures that an image mapped onto a Sphere evaluates with polar 
 * coordinates, while an image mapped onto a Plane evaluates using planar 
 * tiling. Rejects unsupported geometry like groups and CSG nodes.
 * @param object Pointer to the geometric object.
 * @param parser Pointer to the active parsing state.
 * @return       The selected mapping enumeration.
 */
t_uv_map	get_uv_map(t_object *object, t_rt_parser *parser)
{
	if (object->type == SPHERE)
		return (SPHERICAL);
	if (object->type == PLANE)
		return (PLANAR);
	if (object->type == CUBE)
		return (CUBICAL);
	if (object->type == CYLINDER)
		return (CYLINDRICAL);
	if (object->type == CONE)
		return (CONICAL);
	if (object->type == TORUS)
		return (TOROIDAL);
	abort_rt_parse(parser, "UV mapping applied to unsupported geometry",
		RT_PARSE);
	return (TOTAL_UV_MAPS);
}

/**
 * @fn static void parse_uv_chk(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Extracts configuration data for a 2D surface checkerboard.
 * @details Reads the desired tile count for the U and V dimensions, and maps 
 * the 2D plane perfectly around the 3D surface using `get_uv_map()`.
 * @param object Pointer to the geometric object.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_uv_chk(t_object *object, const char **ptr,
	t_rt_parser *parser)
{
	bool	error;
	int		width;
	int		height;
	t_color	colors[2];

	error = false;
	if (parser->tag_flags & TAG_PAT)
		abort_rt_parse(parser, "Multiple patterns found", RT_PARSE);
	parser->tag_flags |= TAG_PAT;
	width = atoi_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	height = atoi_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	colors[0] = parse_color(ptr, &error);
	advance_comma(ptr, &error);
	colors[1] = parse_color(ptr, &error);
	if (error || width <= 0 || height <= 0)
		abort_rt_parse(parser, "Invalid uv_chk formatting", RT_PARSE);
	object->material.pattern = malloc(sizeof(t_pattern));
	if (!object->material.pattern)
		abort_rt_parse(parser, "Malloc failed for uv pattern", MALLOC);
	*(object->material.pattern) = uv_checkers_pattern(width, height,
			get_uv_map(object, parser), colors);
}

/**
 * @fn void parse_texture_tags(t_object *object, const char *key, 
 * const char **ptr, t_rt_parser *parser)
 * @brief Routes specific texture and mapping tags to their sub-extractors.
 * @details Delegates 2D mapped checkerboards, external images, and bump/normal 
 * map file evaluations.
 * @param object Pointer to the geometric object.
 * @param key    The parsed modifier identifier string.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_texture_tags(t_object *object, const char *key,
	const char **ptr, t_rt_parser *parser)
{
	if (!ft_strncmp(key, "uv_chk:", 7))
		parse_uv_chk(object, ptr, parser);
	else if (!ft_strncmp(key, "img:", 4))
		parse_img_map(object, ptr, parser);
	else if (!ft_strncmp(key, "bump:", 5))
		parse_bump_map(object, ptr, parser);
	else
		abort_rt_parse(parser, "Unknown texture tag bypassed router", RT_PARSE);
}
