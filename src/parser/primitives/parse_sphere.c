/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_sphere.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 13:44:56 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/03 15:24:43 by maaugust         ###   ########.fr       */
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
 * @fn void parse_sphere(const char *ptr, t_rt_parser *parser)
 * @brief Extracts configuration data for a Sphere primitive.
 * @details Reads the center point, diameter, and RGB color. Divides the 
 * diameter by 2.0 to derive the radius. Discards spheres with 0 or negative 
 * radii to prevent invisible geometry or infinite intersection loops.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_sphere(const char *ptr, t_rt_parser *parser)
{
	bool		error;
	t_tuple		center;
	double		radius;
	t_color		rgb;
	t_object	*sphere;

	error = false;
	center = parse_tuple(&ptr, &error, 1.0);
	radius = atof_advance(&ptr, &error, false) / 2.0;
	rgb = parse_color(&ptr, &error);
	if (error || radius <= 0.0)
		abort_rt_parse(parser, "Invalid Sphere (sp) formatting", RT_PARSE);
	sphere = create_sphere();
	if (!sphere)
		abort_rt_parse(parser, "Malloc failed for Sphere (sp) object", MALLOC);
	sphere->material.color = rgb;
	append_object(sphere, parser);
	parse_object_tags(sphere, ptr, parser);
	apply_universal_transform(sphere, center, vector(0, 1, 0),
		vector(radius, radius, radius));
}
