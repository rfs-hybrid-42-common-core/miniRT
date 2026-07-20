/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cone.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 13:52:38 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/15 14:31:24 by maaugust         ###   ########.fr       */
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
 * @fn void parse_cone(const char *ptr, t_rt_parser *parser)
 * @brief Extracts configuration data for a Cone primitive.
 * @details Parses positional and geometric parameters from the configuration 
 * file. Applies a scale matrix to the unit cone to stretch its base downward 
 * to match the requested height and radius while maintaining the tip at the 
 * origin.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_cone(const char *ptr, t_rt_parser *parser)
{
	bool		error;
	t_tuple		pos_norm[2];
	double		rad_height[2];
	t_color		rgb;
	t_object	*cone;

	error = false;
	pos_norm[0] = parse_tuple(&ptr, &error, 1.0);
	pos_norm[1] = parse_normal(&ptr, &error);
	rad_height[0] = atof_advance(&ptr, &error, false) / 2.0;
	rad_height[1] = atof_advance(&ptr, &error, false);
	rgb = parse_color(&ptr, &error);
	if (error || rad_height[0] <= 0.0 || rad_height[1] <= 0.0)
		abort_rt_parse(parser, "Invalid Cone (cn) formatting", RT_PARSE);
	cone = create_cone();
	if (!cone)
		abort_rt_parse(parser, "Malloc failed for Cone (cn) object", MALLOC);
	cone->material.color = rgb;
	append_object(cone, parser);
	parse_object_tags(cone, ptr, parser);
	apply_universal_transform(cone, pos_norm[0], pos_norm[1],
		vector(rad_height[0], rad_height[1], rad_height[0]));
}
