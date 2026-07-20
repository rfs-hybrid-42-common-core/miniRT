/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_light_tags.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 15:32:58 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/16 02:55:47 by maaugust         ###   ########.fr       */
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
 * @fn void parse_light_tag_type(t_light *light, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Converts the parsed string identifier into the light type enum.
 * @details Evaluates string sequences ("spot", "area", "point"). Verifies 
 * syntax validity by ensuring the identifier is correctly isolated by 
 * whitespace.
 * @param light  Pointer to the temporary light state.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state for flag tracking.
 */
void	parse_light_tag_type(t_light *light, const char **ptr,
	t_rt_parser *parser)
{
	if (parser->tag_flags & TAG_TYPE)
		abort_rt_parse(parser, "Duplicate Light (L) type tag", RT_PARSE);
	parser->tag_flags |= TAG_TYPE;
	while (ft_isspace(**ptr))
		(*ptr)++;
	if (!ft_strncmp(*ptr, "spot", 4) && (ft_isspace((*ptr)[4]) || !(*ptr)[4]))
	{
		light->type = LIGHT_SPOT;
		*ptr += 4;
	}
	else if (!ft_strncmp(*ptr, "area", 4)
		&& (ft_isspace((*ptr)[4]) || !(*ptr)[4]))
	{
		light->type = LIGHT_AREA;
		*ptr += 4;
	}
	else if (!ft_strncmp(*ptr, "point", 5)
		&& (ft_isspace((*ptr)[5]) || !(*ptr)[5]))
	{
		light->type = LIGHT_POINT;
		*ptr += 5;
	}
	else
		abort_rt_parse(parser, "Invalid Light (L) type value", RT_PARSE);
}

/**
 * @fn void parse_light_tag_direction(t_light *light, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Extracts the explicitly defined direction vector for Spotlights.
 * @details Invokes the internal normal parser, which guarantees the direction 
 * vector will be fully normalized before being attached to the light.
 * @param light  Pointer to the temporary light state.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_light_tag_direction(t_light *light, const char **ptr,
	t_rt_parser *parser)
{
	bool	error;

	error = false;
	if (parser->tag_flags & TAG_DIR)
		abort_rt_parse(parser, "Duplicate Light (L) direction tag", RT_PARSE);
	parser->tag_flags |= TAG_DIR;
	light->direction = parse_normal(ptr, &error);
	if (error || magnitude(light->direction) < EPSILON)
		abort_rt_parse(parser, "Invalid Light (L) direction vector", RT_PARSE);
}

/**
 * @fn void parse_light_tag_cutoff(t_light *light, const char *key, 
 * const char **ptr, t_rt_parser *parser)
 * @brief Extracts an angle parameter (Inner or Outer) for a Spotlight.
 * @details Evaluates the provided angle and triggers a parsing abort if the 
 * physical limits (0.0 to 180.0 degrees) are violated.
 * @param light  Pointer to the temporary light state.
 * @param key    The parsed modifier identifier string.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_light_tag_cutoff(t_light *light, const char *key,
	const char **ptr, t_rt_parser *parser)
{
	bool	error;
	double	angle;
	int		bit;

	error = false;
	bit = 0;
	if (!ft_strncmp(key, "in:", 3))
		bit = TAG_IN;
	else if (!ft_strncmp(key, "out:", 4))
		bit = TAG_OUT;
	else
		abort_rt_parse(parser, "Unknown Light (L) cutoff tag bypassed router",
			RT_PARSE);
	if (parser->tag_flags & bit)
		abort_rt_parse(parser, "Duplicate Light (L) cutoff tag", RT_PARSE);
	parser->tag_flags |= bit;
	angle = atof_advance(ptr, &error, false);
	if (error || angle < 0.0 || angle > 180.0)
		abort_rt_parse(parser, "Invalid Light (L) cutoff angle", RT_PARSE);
	if (bit == TAG_IN)
		light->inner_cutoff = angle;
	else
		light->outer_cutoff = angle;
}

/**
 * @fn void parse_light_tag_steps(t_light *light, const char *key, 
 * const char **ptr, t_rt_parser *parser)
 * @brief Extracts a grid segmentation limit (U or V) for an Area light.
 * @details Rejects 0 or negative integer limits to prevent division by zero 
 * when the Area light constructor evaluates cell width scaling.
 * @param light  Pointer to the temporary light state.
 * @param key    The parsed modifier identifier string.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_light_tag_steps(t_light *light, const char *key,
	const char **ptr, t_rt_parser *parser)
{
	bool	error;
	int		step;
	int		bit;

	error = false;
	bit = 0;
	if (!ft_strncmp(key, "ustep:", 6))
		bit = TAG_USTEP;
	else if (!ft_strncmp(key, "vstep:", 6))
		bit = TAG_VSTEP;
	else
		abort_rt_parse(parser, "Unknown Light (L) step tag bypassed router",
			RT_PARSE);
	if (parser->tag_flags & bit)
		abort_rt_parse(parser, "Duplicate Light (L) step tag", RT_PARSE);
	parser->tag_flags |= bit;
	step = atoi_advance(ptr, &error, false);
	if (error || step <= 0)
		abort_rt_parse(parser, "Invalid Light (L) step value", RT_PARSE);
	if (bit == TAG_USTEP)
		light->usteps = step;
	else
		light->vsteps = step;
}

/**
 * @fn void parse_light_tag_vectors(t_light *light, const char *key, 
 * const char **ptr, t_rt_parser *parser)
 * @brief Extracts a physical dimension vector (U or V) for an Area light.
 * @details Represents the actual size and 3D orientation of the light's 
 * physical geometry. Rejects mathematically void vectors (magnitude = 0).
 * @param light  Pointer to the temporary light state.
 * @param key    The parsed modifier identifier string.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_light_tag_vectors(t_light *light, const char *key,
	const char **ptr, t_rt_parser *parser)
{
	bool	error;
	t_tuple	vector;
	int		bit;

	error = false;
	bit = 0;
	if (!ft_strncmp(key, "uvec:", 5))
		bit = TAG_UVEC;
	else if (!ft_strncmp(key, "vvec:", 5))
		bit = TAG_VVEC;
	else
		abort_rt_parse(parser, "Unknown Light (L) vector tag bypassed router",
			RT_PARSE);
	if (parser->tag_flags & bit)
		abort_rt_parse(parser, "Duplicate Light (L) vector tag", RT_PARSE);
	parser->tag_flags |= bit;
	vector = parse_tuple(ptr, &error, 0.0);
	if (error || magnitude(vector) < EPSILON)
		abort_rt_parse(parser, "Invalid Light (L) vector value", RT_PARSE);
	if (bit == TAG_UVEC)
		light->uvec = vector;
	else
		light->vvec = vector;
}
