/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cylinder.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 21:29:03 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/15 14:31:10 by maaugust         ###   ########.fr       */
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
 * @fn void parse_cylinder(const char *ptr, t_rt_parser *parser)
 * @brief Extracts configuration data for a Cylinder primitive.
 * @details Parses the position, normal, radius, height, and color from the 
 * scene string. The pre-initialized unit cylinder is then uniformly scaled and 
 * translated into its final world-space form via the universal transform.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_cylinder(const char *ptr, t_rt_parser *parser)
{
	bool		error;
	t_tuple		pos_norm[2];
	double		rad_height[2];
	t_color		rgb;
	t_object	*cylinder;

	error = false;
	pos_norm[0] = parse_tuple(&ptr, &error, 1.0);
	pos_norm[1] = parse_normal(&ptr, &error);
	rad_height[0] = atof_advance(&ptr, &error, false) / 2.0;
	rad_height[1] = atof_advance(&ptr, &error, false);
	rgb = parse_color(&ptr, &error);
	if (error || rad_height[0] <= 0.0 || rad_height[1] <= 0.0)
		abort_rt_parse(parser, "Invalid Cylinder (cy) formatting", RT_PARSE);
	cylinder = create_cylinder();
	if (!cylinder)
		abort_rt_parse(parser, "Malloc failed for Cylinder (cy) object",
			MALLOC);
	cylinder->material.color = rgb;
	append_object(cylinder, parser);
	parse_object_tags(cylinder, ptr, parser);
	apply_universal_transform(cylinder, pos_norm[0], pos_norm[1],
		vector(rad_height[0], rad_height[1], rad_height[0]));
}
