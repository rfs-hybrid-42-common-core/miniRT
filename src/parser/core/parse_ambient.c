/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_ambient.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/21 21:29:26 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 03:16:32 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "parser.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn void parse_ambient(const char *ptr, t_rt_parser *parser)
 * @brief Extracts global ambient lighting parameters.
 * @details Ensures the 'A' tag only appears once in the file. Extracts the 
 * intensity ratio and base RGB color, validating that the ratio falls strictly 
 * between 0.0 and 1.0. Checks for trailing garbage data to enforce strict 
 * syntax.
 * @param ptr    Double pointer to the raw string block.
 * @param parser Pointer to the active parsing state.
 */
void	parse_ambient(const char *ptr, t_rt_parser *parser)
{
	bool	error;
	double	ratio;
	t_color	rgb;

	error = false;
	if (parser->has_ambient)
		abort_rt_parse(parser, "Multiple Ambient lights (A) found", RT_PARSE);
	ratio = atof_advance(&ptr, &error, false);
	rgb = parse_color(&ptr, &error);
	if (error || ratio < 0.0 || ratio > 1.0 || has_trailing_garbage(ptr))
		abort_rt_parse(parser, "Invalid Ambient light (A) formatting",
			RT_PARSE);
	parser->engine->world.ambient_ratio = ratio;
	parser->engine->world.ambient_color = rgb;
	parser->has_ambient = true;
}
