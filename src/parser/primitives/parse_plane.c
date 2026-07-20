/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_plane.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 02:30:36 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/03 15:24:23 by maaugust         ###   ########.fr       */
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
 * @fn void parse_plane(const char *ptr, t_rt_parser *parser)
 * @brief Extracts configuration data for an infinite Plane primitive.
 * @details Reads the anchor coordinate, the orientation normal vector, and 
 * the RGB color. The normal parser inherently guarantees the orientation 
 * vector is correctly normalized.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_plane(const char *ptr, t_rt_parser *parser)
{
	bool		error;
	t_tuple		position;
	t_tuple		normal;
	t_color		rgb;
	t_object	*plane;

	error = false;
	position = parse_tuple(&ptr, &error, 1.0);
	normal = parse_normal(&ptr, &error);
	rgb = parse_color(&ptr, &error);
	if (error)
		abort_rt_parse(parser, "Invalid Plane (pl) formatting", RT_PARSE);
	plane = create_plane();
	if (!plane)
		abort_rt_parse(parser, "Malloc failed for Plane (pl) object", MALLOC);
	plane->material.color = rgb;
	append_object(plane, parser);
	parse_object_tags(plane, ptr, parser);
	apply_universal_transform(plane, position, normal, vector(1, 1, 1));
}
