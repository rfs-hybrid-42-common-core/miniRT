/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_environment_tags.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 20:40:31 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 03:35:30 by maaugust         ###   ########.fr       */
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
 * @fn static void set_emissive_material(t_material *material)
 * @brief Strips light-reaction physics from a material.
 * @details By forcing ambient response to 100% and stripping out 
 * diffuse/specular interaction, the assigned texture will render exactly as it 
 * was painted in the file, ignoring all environmental shadows. Critical for 
 * making skyboxes and HDRI domes function correctly as backgrounds.
 * @param material Pointer to the material to modify.
 */
static void	set_emissive_material(t_material *material)
{
	material->ambient = 1.0;
	material->diffuse = 0.0;
	material->specular = 0.0;
}

/**
 * @fn static void parse_dome(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Binds a panoramic HDRI texture to a massive spherical bounding box.
 * @details Verifies the parent object is physically a Sphere. Loads the XPM 
 * image map, enforces spherical polar UV unwrapping, and modifies the material 
 * to ignore incoming shadows.
 * @param object Pointer to the bounding geometric object.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_dome(t_object *object, const char **ptr, t_rt_parser *parser)
{
	char		path[MAX_PATH_LEN];
	t_pattern	*pattern;

	if (object->type != SPHERE || parser->tag_flags & TAG_PAT)
		abort_rt_parse(parser, "Invalid dome object or multiple patterns found",
			RT_PARSE);
	parser->tag_flags |= TAG_PAT;
	if (get_filepath(ptr, path, '\0'))
		abort_rt_parse(parser, "Empty dome filepath provided", XPM_OPEN);
	if (!has_xpm_extension(path))
		abort_rt_parse(parser, "Invalid dome map extension (expected .xpm)",
			XPM_OPEN);
	pattern = malloc(sizeof(t_pattern));
	if (!pattern)
		abort_rt_parse(parser, "Malloc failed for dome pattern", MALLOC);
	*pattern = uv_image_pattern(parser->engine->mlx.mlx_ptr, path, SPHERICAL);
	object->material.pattern = pattern;
	set_emissive_material(&object->material);
}

/**
 * @fn static void parse_skybox(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Binds 6 textures to the respective inner faces of a bounding Cube.
 * @details Verifies the parent object is physically a Cube. Parses the 6 
 * comma-separated texture paths (Right, Left, Up, Down, Front, Back) and 
 * constructs a specialized environment map that completely surrounds the scene.
 * @param object Pointer to the bounding geometric object.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_skybox(t_object *object, const char **ptr,
	t_rt_parser *parser)
{
	char		buffer[6][MAX_PATH_LEN];
	char		*paths[6];
	t_pattern	*pattern;
	int			i;

	if (object->type != CUBE || parser->tag_flags & TAG_PAT)
		abort_rt_parse(parser,
			"Invalid skybox object or multiple patterns found", RT_PARSE);
	parser->tag_flags |= TAG_PAT;
	i = -1;
	while (++i < 6)
	{
		if ((i < 5 && get_filepath(ptr, buffer[i], ','))
			|| (i == 5 && get_filepath(ptr, buffer[i], '\0')))
			abort_rt_parse(parser, "Empty skybox filepath provided", XPM_OPEN);
		if (!has_xpm_extension(buffer[i]))
			abort_rt_parse(parser, "Invalid skybox map extension", XPM_OPEN);
		paths[i] = buffer[i];
	}
	pattern = malloc(sizeof(t_pattern));
	if (!pattern)
		abort_rt_parse(parser, "Malloc failed for skybox pattern", MALLOC);
	*pattern = uv_skybox_pattern(parser->engine->mlx.mlx_ptr, paths);
	object->material.pattern = pattern;
	set_emissive_material(&object->material);
}

/**
 * @fn void parse_environment_tags(t_object *object, const char *key, 
 * const char **ptr, t_rt_parser *parser)
 * @brief Routes specific environmental background tags to their sub-extractors.
 * @details Delegates texture assignments intended specifically to bypass 
 * standard shading, rendering flatly behind all other geometry.
 * @param object Pointer to the bounding geometric object.
 * @param key    The parsed modifier identifier string.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_environment_tags(t_object *object, const char *key,
	const char **ptr, t_rt_parser *parser)
{
	if (!ft_strncmp(key, "dome:", 5))
		parse_dome(object, ptr, parser);
	else if (!ft_strncmp(key, "skybox:", 7))
		parse_skybox(object, ptr, parser);
	else
		abort_rt_parse(parser, "Unknown environment tag bypassed router",
			RT_PARSE);
}
