/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cube.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 16:51:57 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/03 15:24:55 by maaugust         ###   ########.fr       */
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
 * @fn void parse_cube(const char *ptr, t_rt_parser *parser)
 * @brief Extracts configuration data for a Cube primitive.
 * @details Reads position, normal orientation, side length, and color. Divides 
 * the side length by 2 to obtain the center-to-edge radius required for proper 
 * matrix scaling of the baseline unit AABB.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_cube(const char *ptr, t_rt_parser *parser)
{
	bool		error;
	t_tuple		pos_norm[2];
	double		length;
	t_color		rgb;
	t_object	*cube;

	error = false;
	pos_norm[0] = parse_tuple(&ptr, &error, 1.0);
	pos_norm[1] = parse_normal(&ptr, &error);
	length = atof_advance(&ptr, &error, false) / 2.0;
	rgb = parse_color(&ptr, &error);
	if (error || length <= 0.0)
		abort_rt_parse(parser, "Invalid Cube (cb) formatting", RT_PARSE);
	cube = create_cube();
	if (!cube)
		abort_rt_parse(parser, "Malloc failed for Cube (cb) object", MALLOC);
	cube->material.color = rgb;
	append_object(cube, parser);
	parse_object_tags(cube, ptr, parser);
	apply_universal_transform(cube, pos_norm[0], pos_norm[1],
		vector(length, length, length));
}
