/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_procedural_tags.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 00:46:06 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/02 00:29:46 by maaugust         ###   ########.fr       */
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
 * @fn static void allocate_pattern(t_object *object, t_rt_parser *parser)
 * @brief Safely provisions memory for a new procedural pattern.
 * @details Checks the global pattern bitmask to guarantee an object doesn't 
 * attempt to stack multiple base patterns (like blending stripes with 
 * checkers). Clears the allocated memory to zero to avoid uninitialized 
 * variable bugs.
 * @param object Pointer to the geometric object receiving the pattern.
 * @param parser Pointer to the active parsing state.
 */
static void	allocate_pattern(t_object *object, t_rt_parser *parser)
{
	if (parser->tag_flags & TAG_PAT)
		abort_rt_parse(parser, "Multiple patterns found", RT_PARSE);
	parser->tag_flags |= TAG_PAT;
	object->material.pattern = malloc(sizeof(t_pattern));
	if (!object->material.pattern)
		abort_rt_parse(parser, "Malloc failed for object pattern", MALLOC);
	ft_bzero(object->material.pattern, sizeof(t_pattern));
}

/**
 * @fn static void parse_basic_pattern(t_object *object, const char *key, 
 * const char **ptr, t_rt_parser *parser)
 * @brief Extracts colors for purely geometric procedural patterns.
 * @details Handles the parameter extraction for 1D stripes, linear gradients, 
 * radial rings, and 3D volumetric checkers, routing to the correct initializer.
 * @param object Pointer to the geometric object.
 * @param key    The parsed modifier identifier string.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_basic_pattern(t_object *object, const char *key,
	const char **ptr, t_rt_parser *parser)
{
	bool	error;
	t_color	color_a;
	t_color	color_b;

	error = false;
	allocate_pattern(object, parser);
	color_a = parse_color(ptr, &error);
	advance_comma(ptr, &error);
	color_b = parse_color(ptr, &error);
	if (error)
		abort_rt_parse(parser, "Invalid basic pattern formatting", RT_PARSE);
	if (!ft_strncmp(key, "str:", 4))
		*(object->material.pattern) = stripe_pattern(color_a, color_b);
	else if (!ft_strncmp(key, "gra:", 4))
		*(object->material.pattern) = gradient_pattern(color_a, color_b);
	else if (!ft_strncmp(key, "rng:", 4))
		*(object->material.pattern) = ring_pattern(color_a, color_b);
	else if (!ft_strncmp(key, "chk:", 4))
		*(object->material.pattern) = checkers_pattern(color_a, color_b);
	else
		abort_rt_parse(parser, "Unknown basic pattern bypassed router",
			RT_PARSE);
}

/**
 * @fn static void parse_perlin_pattern(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Extracts configuration data for 3D fractal noise.
 * @details Reads the two base colors, the fractal octave depth, and 
 * importantly, a scaling multiplier. The scaling multiplier constructs a 
 * scaling_matrix to shrink or enlarge the noise space relative to the 
 * object's local geometry.
 * @param object Pointer to the geometric object.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_perlin_pattern(t_object *object, const char **ptr,
	t_rt_parser *parser)
{
	bool		error;
	t_color		colors[2];
	int			octaves;
	double		scale;
	t_matrix	pattern_scale;

	error = false;
	allocate_pattern(object, parser);
	colors[0] = parse_color(ptr, &error);
	advance_comma(ptr, &error);
	colors[1] = parse_color(ptr, &error);
	advance_comma(ptr, &error);
	octaves = atoi_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	scale = atof_advance(ptr, &error, false);
	if (error || octaves < 1 || scale <= 0.0)
		abort_rt_parse(parser, "Invalid perlin pattern formatting", RT_PARSE);
	*(object->material.pattern) = perlin_pattern(colors[0], colors[1], octaves);
	pattern_scale = scaling_matrix(scale, scale, scale);
	set_pattern_transform(object->material.pattern, &pattern_scale);
}

/**
 * @fn static void parse_marble_pattern(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Extracts configuration data for procedural stone.
 * @details Extremely similar to Perlin, but extracts an additional 
 * 'vein_scale' parameter dictating the frequency of the underlying 
 * sine wave generator.
 * @param object Pointer to the geometric object.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_marble_pattern(t_object *object, const char **ptr,
	t_rt_parser *parser)
{
	bool		error;
	t_color		colors[2];
	int			octaves;
	double		scale[2];
	t_matrix	pattern_scale;

	error = false;
	allocate_pattern(object, parser);
	colors[0] = parse_color(ptr, &error);
	advance_comma(ptr, &error);
	colors[1] = parse_color(ptr, &error);
	advance_comma(ptr, &error);
	octaves = atoi_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	scale[0] = atof_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	scale[1] = atof_advance(ptr, &error, false);
	if (error || octaves < 1 || scale[0] <= 0.0 || scale[1] <= 0.0)
		abort_rt_parse(parser, "Invalid marble pattern formatting", RT_PARSE);
	*(object->material.pattern) = marble_pattern(colors[0], colors[1], octaves,
			scale[0]);
	pattern_scale = scaling_matrix(scale[1], scale[1], scale[1]);
	set_pattern_transform(object->material.pattern, &pattern_scale);
}

/**
 * @fn void parse_procedural_tags(t_object *object, const char *key, 
 * const char **ptr, t_rt_parser *parser)
 * @brief Routes specific procedural pattern tags to their sub-extractors.
 * @details Delegates mathematical noise and color-blending pattern evaluation.
 * @param object Pointer to the geometric object.
 * @param key    The parsed modifier identifier string.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_procedural_tags(t_object *object, const char *key,
	const char **ptr, t_rt_parser *parser)
{
	if (!ft_strncmp(key, "str:", 4) || !ft_strncmp(key, "gra:", 4)
		|| !ft_strncmp(key, "rng:", 4) || !ft_strncmp(key, "chk:", 4))
		parse_basic_pattern(object, key, ptr, parser);
	else if (!ft_strncmp(key, "perlin:", 7))
		parse_perlin_pattern(object, ptr, parser);
	else if (!ft_strncmp(key, "marble:", 7))
		parse_marble_pattern(object, ptr, parser);
	else
		abort_rt_parse(parser, "Unknown procedural tag bypassed router",
			RT_PARSE);
}
