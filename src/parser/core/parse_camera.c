/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_camera.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 02:28:48 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 03:18:18 by maaugust         ###   ########.fr       */
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
 * @fn static void setup_camera_view(t_camera *camera, t_tuple position, 
 * t_tuple direction)
 * @brief Calculates and assigns the primary camera transformation matrix.
 * @details Derives the true orientation of the camera using cross products. 
 * Safely handles gimbal lock (when the camera looks directly up or down) by 
 * swapping the default (0,1,0) UP vector to a (0,0,1) UP vector.
 * @param camera    Pointer to the active camera struct.
 * @param position  The spatial origin of the lens.
 * @param direction The target orientation vector.
 */
static void	setup_camera_view(t_camera *camera, t_tuple position,
	t_tuple direction)
{
	t_tuple		up;
	t_matrix	view;

	direction = normalize(direction);
	if (fabs(direction.x) < EPSILON && fabs(direction.z) < EPSILON)
		up = vector(0, 0, 1);
	else
		up = vector(0, 1, 0);
	view = view_transform(position, add_tuples(position, direction), up);
	set_camera_transform(camera, &view);
}

/**
 * @fn static void route_camera_tag(t_camera *camera, const char *key, 
 * const char **ptr, t_rt_parser *parser)
 * @brief Evaluates an optional camera modifier string block.
 * @details Intercepts "ap:" and "focal:" strings, verifies they haven't 
 * already been declared using the parser's bitmask flag, and assigns them. 
 * Triggers an abort on unrecognized modifiers.
 * @param camera Pointer to the active camera struct.
 * @param key    The parsed modifier identifier string.
 * @param ptr    Double pointer to the associated argument values.
 * @param parser Pointer to the active parsing state.
 */
static void	route_camera_tag(t_camera *camera, const char *key,
	const char **ptr, t_rt_parser *parser)
{
	bool	error;

	error = false;
	if (!ft_strncmp(key, "ap:", 3))
	{
		if (parser->tag_flags & TAG_AP)
			abort_rt_parse(parser, "Duplicate Camera (C) aperture tag",
				RT_PARSE);
		camera->aperture = atof_advance(ptr, &error, false);
		if (error || camera->aperture < 0.0)
			abort_rt_parse(parser, "Invalid Camera (C) aperture tag", RT_PARSE);
		parser->tag_flags |= TAG_AP;
	}
	else if (!ft_strncmp(key, "focal:", 6))
	{
		if (parser->tag_flags & TAG_FOCAL)
			abort_rt_parse(parser, "Duplicate Camera (C) focal tag", RT_PARSE);
		camera->focal_length = atof_advance(ptr, &error, false);
		if (error || camera->focal_length <= 0.0)
			abort_rt_parse(parser, "Invalid Camera (C) focal tag", RT_PARSE);
		parser->tag_flags |= TAG_FOCAL;
	}
	else
		abort_rt_parse(parser, "Unknown bonus tag on Camera (C)", RT_PARSE);
}

/**
 * @fn static void parse_camera_tags(t_camera *camera, const char *ptr, 
 * t_rt_parser *parser)
 * @brief Iterates through all trailing modifier tags for the camera.
 * @details Skips whitespace, isolates the alphanumeric modifier key ending 
 * with a colon, and advances the pointer directly to the values. Terminates 
 * parsing when it reaches the end of the line or hits a comment '#'.
 * @param camera Pointer to the active camera struct.
 * @param ptr    The string containing the remaining modifiers.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_camera_tags(t_camera *camera, const char *ptr,
	t_rt_parser *parser)
{
	char	key[MAX_TAG_LEN];
	int		i;

	parser->tag_flags = 0;
	while (*ptr)
	{
		while (ft_isspace(*ptr))
			ptr++;
		if (!*ptr || *ptr == '\n' || *ptr == '#')
			break ;
		i = 0;
		while (ptr[i] && !ft_isspace(ptr[i]) && ptr[i] != ':'
			&& i < MAX_TAG_LEN - 2)
			i++;
		if (ptr[i] == ':')
			i++;
		ft_strlcpy(key, ptr, i + 1);
		ptr += i;
		route_camera_tag(camera, key, &ptr, parser);
	}
}

/**
 * @fn void parse_camera(const char *ptr, t_rt_parser *parser)
 * @brief Master extraction function for the Camera ('C') element.
 * @details Verifies uniqueness. Pulls the absolute coordinates, viewing 
 * direction, and Field of View (FOV). Then converts the FOV into radians, 
 * initializes the base camera mathematically, and delegates the remaining 
 * string parsing to the modifier loops.
 * @param ptr    Double pointer to the raw string block.
 * @param parser Pointer to the active parsing state.
 */
void	parse_camera(const char *ptr, t_rt_parser *parser)
{
	bool		error;
	t_tuple		position;
	t_tuple		direction;
	int			fov;

	error = false;
	if (parser->has_camera)
		abort_rt_parse(parser, "Multiple Cameras (C) found", RT_PARSE);
	position = parse_tuple(&ptr, &error, 1.0);
	direction = parse_tuple(&ptr, &error, 0.0);
	fov = atoi_advance(&ptr, &error, false);
	if (error || fov < 0 || fov > 180 || magnitude(direction) < EPSILON
		|| !validate_vector_range(direction))
		abort_rt_parse(parser, "Invalid Camera (C) formatting", RT_PARSE);
	parser->engine->camera = create_camera(parser->engine->camera.hsize,
			parser->engine->camera.vsize, (double)fov * (M_PI / 180.0));
	setup_camera_view(&parser->engine->camera, position, direction);
	parse_camera_tags(&parser->engine->camera, ptr, parser);
	parser->has_camera = true;
}
