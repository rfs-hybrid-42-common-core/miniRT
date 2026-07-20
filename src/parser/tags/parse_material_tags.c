/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_material_tags.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 18:52:12 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/16 03:52:55 by maaugust         ###   ########.fr       */
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
 * @fn static void parse_mat_override(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Extracts standard Phong shading multipliers.
 * @details Replaces the default material parameters with explicit ambient, 
 * diffuse, specular, and shininess values. Validates that the light ratios 
 * strictly rest within the [0.0, 1.0] constraint.
 * @param object Pointer to the geometric object.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_mat_override(t_object *object, const char **ptr,
	t_rt_parser *parser)
{
	bool	error;

	error = false;
	if (parser->tag_flags & TAG_MAT)
		abort_rt_parse(parser, "Duplicate object mat tag", RT_PARSE);
	parser->tag_flags |= TAG_MAT;
	object->material.ambient = atof_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	object->material.diffuse = atof_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	object->material.specular = atof_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	object->material.shininess = atof_advance(ptr, &error, false);
	if (error || object->material.shininess < 0.0
		|| object->material.ambient < 0.0 || object->material.ambient > 1.0
		|| object->material.diffuse < 0.0 || object->material.diffuse > 1.0
		|| object->material.specular < 0.0 || object->material.specular > 1.0)
		abort_rt_parse(parser, "Invalid mat formatting or range", RT_PARSE);
}

/**
 * @fn static void parse_ref_override(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Extracts the reflection ratio.
 * @details Converts a standard matte material into a mirror surface. The 
 * ratio determines how much environmental light is reflected versus the 
 * object's intrinsic base color.
 * @param object Pointer to the geometric object.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_ref_override(t_object *object, const char **ptr,
	t_rt_parser *parser)
{
	bool	error;

	error = false;
	if (parser->tag_flags & TAG_REF)
		abort_rt_parse(parser, "Duplicate object ref tag", RT_PARSE);
	parser->tag_flags |= TAG_REF;
	object->material.reflective = atof_advance(ptr, &error, false);
	if (error || object->material.reflective < 0.0
		|| object->material.reflective > 1.0)
		abort_rt_parse(parser, "Invalid ref ratio or range (0.0 - 1.0)",
			RT_PARSE);
}

/**
 * @fn static void parse_glass_override(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Extracts refraction and transparency parameters.
 * @details Turns solid geometry into glass or water. Ensures the Index of 
 * Refraction (IOR) is physically valid (>= 1.0, representing the vacuum of 
 * space) and mathematically safe (<= 5.0) to prevent floating-point precision 
 * loss.
 * @param object Pointer to the geometric object.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_glass_override(t_object *object, const char **ptr,
	t_rt_parser *parser)
{
	bool	error;

	error = false;
	if (parser->tag_flags & TAG_GLASS)
		abort_rt_parse(parser, "Duplicate object glass tag", RT_PARSE);
	parser->tag_flags |= TAG_GLASS;
	object->material.refractive_index = atof_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	object->material.transparency = atof_advance(ptr, &error, false);
	if (error || object->material.refractive_index < 1.0
		|| object->material.refractive_index > 5.0
		|| object->material.transparency < 0.0
		|| object->material.transparency > 1.0)
		abort_rt_parse(parser, "Invalid glass properties or bounds", RT_PARSE);
}

/**
 * @fn void parse_material_tags(t_object *object, const char *key, 
 * const char **ptr, t_rt_parser *parser)
 * @brief Routes specific material property tags to their sub-extractors.
 * @details Delegates "mat:", "ref:", and "glass:" parsing.
 * @param object Pointer to the geometric object.
 * @param key    The parsed modifier identifier string.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_material_tags(t_object *object, const char *key,
	const char **ptr, t_rt_parser *parser)
{
	if (!ft_strncmp(key, "mat:", 4))
		parse_mat_override(object, ptr, parser);
	else if (!ft_strncmp(key, "ref:", 4))
		parse_ref_override(object, ptr, parser);
	else if (!ft_strncmp(key, "glass:", 6))
		parse_glass_override(object, ptr, parser);
	else
		abort_rt_parse(parser, "Unknown material tag bypassed router",
			RT_PARSE);
}
