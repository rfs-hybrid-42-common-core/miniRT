/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_xpm_file.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 15:55:43 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/12 22:44:37 by maaugust         ###   ########.fr       */
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
 * @fn void parse_img_map(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Extracts the file path to an external texture and verifies extension.
 * @details Scans the string for the .xpm extension and asserts the pattern mask 
 * to ensure images do not stack on top of procedural patterns. Validates the 
 * object's geometry for UV mapping support prior to allocating the pattern 
 * memory structure and loading the texture via MLX.
 * @param object Pointer to the geometric object.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_img_map(t_object *object, const char **ptr, t_rt_parser *parser)
{
	char		path[MAX_PATH_LEN];
	t_pattern	*pattern;
	t_uv_map	uv_map;

	if (parser->tag_flags & TAG_PAT)
		abort_rt_parse(parser, "Multiple patterns found", RT_PARSE);
	parser->tag_flags |= TAG_PAT;
	if (get_filepath(ptr, path, '\0'))
		abort_rt_parse(parser, "Empty image filepath provided", XPM_OPEN);
	if (!has_xpm_extension(path))
		abort_rt_parse(parser, "Invalid image map extension (expected .xpm)",
			XPM_OPEN);
	uv_map = get_uv_map(object, parser);
	pattern = malloc(sizeof(t_pattern));
	if (!pattern)
		abort_rt_parse(parser, "Malloc failed for image map", MALLOC);
	*pattern = uv_image_pattern(parser->engine->mlx.mlx_ptr, path, uv_map);
	object->material.pattern = pattern;
}

/**
 * @fn static void parse_bump_parameters(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Extracts and validates the numerical parameters for a bump map.
 * @details Parses the bump strength and epsilon values from the configuration 
 * string, ensuring they are properly formatted and strictly positive.
 * @param object Pointer to the geometric object being textured.
 * @param ptr    Double pointer to the raw string argument, advanced during 
 * extraction.
 * @param parser Pointer to the active parsing state for error handling.
 */
static void	parse_bump_parameters(t_object *object, const char **ptr,
	t_rt_parser *parser)
{
	bool	error;

	error = false;
	object->material.bump_strength = atof_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	object->material.bump_epsilon = atof_advance(ptr, &error, false);
	if (error || object->material.bump_strength <= 0.0
		|| object->material.bump_epsilon <= 0.0)
		abort_rt_parse(parser, "Invalid bump/normal map formatting", RT_PARSE);
}

/**
 * @fn void parse_bump_map(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Extracts the file path and mathematical strength of a normal/height 
 * map.
 * @details Bump maps utilize a separate bitmask flag (`TAG_BUMP`), allowing an 
 * object to possess both a physical image texture (`TAG_PAT`) and a bump map 
 * simultaneously. Retrieves the map parameters and validates the geometry's UV 
 * mapping compatibility before allocating and assigning the pattern structure.
 * @param object Pointer to the geometric object.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_bump_map(t_object *object, const char **ptr, t_rt_parser *parser)
{
	char		path[MAX_PATH_LEN];
	t_pattern	*pattern;
	t_uv_map	uv_map;

	if (parser->tag_flags & TAG_BUMP)
		abort_rt_parse(parser, "Duplicate object bump tag", RT_PARSE);
	parser->tag_flags |= TAG_BUMP;
	if (get_filepath(ptr, path, ','))
		abort_rt_parse(parser, "Empty bump/normal file path provided",
			RT_PARSE);
	if (!has_xpm_extension(path))
		abort_rt_parse(parser,
			"Invalid bump/normal map extension (expected .xpm)", XPM_OPEN);
	parse_bump_parameters(object, ptr, parser);
	uv_map = get_uv_map(object, parser);
	pattern = malloc(sizeof(t_pattern));
	if (!pattern)
		abort_rt_parse(parser, "Malloc failed for bump/normal map", MALLOC);
	*pattern = uv_image_pattern(parser->engine->mlx.mlx_ptr, path, uv_map);
	object->material.bump_map = pattern;
}
