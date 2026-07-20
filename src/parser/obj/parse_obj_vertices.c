/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_obj_vertices.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 15:41:19 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 03:54:45 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"
#include "parser.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stdlib.h>
#include <stddef.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn void parse_v(char *line, t_obj_parser *parser)
 * @brief Extracts a spatial vertex coordinate to the pre-allocated array.
 * @details Reads the X, Y, and Z parameters and formats them into a point 
 * tuple (w=1.0).
 * @param line   The string line starting with 'v '.
 * @param parser Pointer to the OBJ parsing state.
 */
void	parse_v(char *line, t_obj_parser *parser)
{
	const char	*ptr;
	bool		error;

	error = false;
	ptr = line + 1;
	parser->v_array[parser->v_index].x = atof_advance(&ptr, &error, false);
	parser->v_array[parser->v_index].y = atof_advance(&ptr, &error, false);
	parser->v_array[parser->v_index].z = atof_advance(&ptr, &error, false);
	parser->v_array[parser->v_index].w = 1.0;
	if (error || has_trailing_garbage(ptr))
		abort_obj_parse(parser, "Malformed vertex data in OBJ file", NULL,
			OBJ_PARSE);
	parser->v_index++;
}

/**
 * @fn void parse_vt(char *line, t_obj_parser *parser)
 * @brief Extracts a texture coordinate to the pre-allocated array.
 * @details Reads the U and V coordinates. Safely checks for the optional 
 * third coordinate (W), allowing it only if it is mathematically negligible 
 * (0.0).
 * @param line   The string line starting with 'vt '.
 * @param parser Pointer to the OBJ parsing state.
 */
void	parse_vt(char *line, t_obj_parser *parser)
{
	const char	*ptr;
	bool		error;

	error = false;
	ptr = line + 2;
	parser->vt_array[parser->vt_index].x = atof_advance(&ptr, &error, false);
	parser->vt_array[parser->vt_index].y = atof_advance(&ptr, &error, false);
	parser->vt_array[parser->vt_index].z = 0.0;
	while (ft_isspace(*ptr))
		ptr++;
	if (*ptr && *ptr != '\n' && *ptr != '#')
	{
		parser->vt_array[parser->vt_index].z = atof_advance(&ptr, &error,
				false);
		if (fabs(parser->vt_array[parser->vt_index].z) > EPSILON)
			error = true;
	}
	parser->vt_array[parser->vt_index].w = 0.0;
	if (error || has_trailing_garbage(ptr))
		abort_obj_parse(parser, "Malformed vertex texture data in OBJ file",
			NULL, OBJ_PARSE);
	parser->vt_index++;
}

/**
 * @fn void parse_vn(char *line, t_obj_parser *parser)
 * @brief Extracts an explicit vertex normal to the pre-allocated array.
 * @details Reads the X, Y, and Z direction parameters and stores them as 
 * a pure vector tuple (w=0.0).
 * @param line   The string line starting with 'vn '.
 * @param parser Pointer to the OBJ parsing state.
 */
void	parse_vn(char *line, t_obj_parser *parser)
{
	const char	*ptr;
	bool		error;

	error = false;
	ptr = line + 2;
	parser->vn_array[parser->vn_index].x = atof_advance(&ptr, &error, false);
	parser->vn_array[parser->vn_index].y = atof_advance(&ptr, &error, false);
	parser->vn_array[parser->vn_index].z = atof_advance(&ptr, &error, false);
	parser->vn_array[parser->vn_index].w = 0.0;
	if (error || has_trailing_garbage(ptr))
		abort_obj_parse(parser, "Malformed vertex normal data in OBJ file",
			NULL, OBJ_PARSE);
	parser->vn_index++;
}
