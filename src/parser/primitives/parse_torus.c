/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_torus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 16:59:05 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/03 15:25:13 by maaugust         ###   ########.fr       */
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
 * @fn void parse_torus(const char *ptr, t_rt_parser *parser)
 * @brief Extracts configuration data for a Torus primitive.
 * @details Reads the position, normal, major diameter, minor diameter, and 
 * color. 
 * Converts both diameters to radii. Rejects instances where the minor radius 
 * exceeds the major radius (which mathematically inverts the donut).
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_torus(const char *ptr, t_rt_parser *parser)
{
	bool		error;
	t_tuple		pos_norm[2];
	double		major_minor_r[2];
	t_color		rgb;
	t_object	*torus;

	error = false;
	pos_norm[0] = parse_tuple(&ptr, &error, 1.0);
	pos_norm[1] = parse_normal(&ptr, &error);
	major_minor_r[0] = atof_advance(&ptr, &error, false) / 2.0;
	major_minor_r[1] = atof_advance(&ptr, &error, false) / 2.0;
	rgb = parse_color(&ptr, &error);
	if (error || major_minor_r[0] <= 0.0 || major_minor_r[1] <= 0.0
		|| (major_minor_r[0] - major_minor_r[1]) <= 0.0)
		abort_rt_parse(parser, "Invalid Torus (to) formatting", RT_PARSE);
	torus = create_torus(major_minor_r[0], major_minor_r[1]);
	if (!torus)
		abort_rt_parse(parser, "Malloc failed for Torus (to) object", MALLOC);
	torus->material.color = rgb;
	append_object(torus, parser);
	parse_object_tags(torus, ptr, parser);
	apply_universal_transform(torus, pos_norm[0], pos_norm[1],
		vector(1, 1, 1));
}
