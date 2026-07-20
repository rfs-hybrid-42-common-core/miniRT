/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_resolution.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/21 14:19:40 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 03:16:56 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "mlx.h"
#include "parser.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn void parse_resolution(const char *ptr, t_rt_parser *parser)
 * @brief Extracts the render canvas dimensions.
 * @details The resolution tag must appear before the camera, as the camera 
 * relies on the aspect ratio for its internal mathematical projection. 
 * Utilizes the MLX API to check the user's native monitor size, clamping the 
 * resolution so it cannot exceed the physical screen limits.
 * @param ptr    Double pointer to the raw string block.
 * @param parser Pointer to the active parsing state.
 */
void	parse_resolution(const char *ptr, t_rt_parser *parser)
{
	bool	error;
	int		width;
	int		height;
	int		max_width;
	int		max_height;

	error = false;
	if (parser->has_resolution)
		abort_rt_parse(parser, "Multiple Resolutions (R) found", RT_PARSE);
	if (parser->has_camera)
		abort_rt_parse(parser,
			"Resolution (R) must be declared before Camera (C)", RT_PARSE);
	width = atoi_advance(&ptr, &error, false);
	height = atoi_advance(&ptr, &error, false);
	if (error || width <= 0 || height <= 0 || has_trailing_garbage(ptr))
		abort_rt_parse(parser, "Invalid resolution (R) formatting", RT_PARSE);
	mlx_get_screen_size(parser->engine->mlx.mlx_ptr, &max_width, &max_height);
	if (width > max_width)
		width = max_width;
	if (height > max_height)
		height = max_height;
	parser->engine->camera.hsize = width;
	parser->engine->camera.vsize = height;
	parser->has_resolution = true;
}
