/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_csg.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 20:34:09 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/03 15:27:20 by maaugust         ###   ########.fr       */
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
 * @fn static void parse_csg_headers(const char **ptr, t_object *csg, 
 * t_rt_parser *parser)
 * @brief Decodes the operational parameters for Constructive Solid Geometry.
 * @details Reads the literal boolean operation (union, inter, diff) and the 
 * two explicit string IDs referencing the objects that will serve as the left 
 * and right operands in the shape intersection logic.
 * @param ptr    Double pointer to the raw string argument.
 * @param csg    Pointer to the allocated CSG logical node.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_csg_headers(const char **ptr, t_object *csg,
	t_rt_parser *parser)
{
	char	buffer[MAX_MODIFIER_LEN];

	extract_token(ptr, buffer, MAX_MODIFIER_LEN);
	if (!ft_strncmp(buffer, "inter", 6))
		csg->data.csg.operation = CSG_INTERSECTION;
	else if (!ft_strncmp(buffer, "diff", 5))
		csg->data.csg.operation = CSG_DIFFERENCE;
	else if (ft_strncmp(buffer, "union", 6))
		abort_rt_parse(parser, "Invalid CSG op (expected union, inter, diff)",
			RT_PARSE);
	extract_token(ptr, buffer, MAX_MODIFIER_LEN);
	csg->data.csg.left_id = ft_strdup(buffer);
	extract_token(ptr, buffer, MAX_MODIFIER_LEN);
	csg->data.csg.right_id = ft_strdup(buffer);
	if (!csg->data.csg.left_id || !csg->data.csg.right_id
		|| !csg->data.csg.left_id[0] || !csg->data.csg.right_id[0])
		abort_rt_parse(parser, "Invalid or missing CSG child IDs", RT_PARSE);
}

/**
 * @fn void parse_csg(const char *ptr, t_rt_parser *parser)
 * @brief Extracts configuration data for a Constructive Solid Geometry node.
 * @details Spawns the logical node and parses its headers and transformation 
 * properties. The object is appended to the flat list; it will not physically 
 * connect to its left/right children until the parse_linker phase activates.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_csg(const char *ptr, t_rt_parser *parser)
{
	bool		error;
	t_tuple		position;
	t_tuple		normal;
	double		scale;
	t_object	*csg;

	error = false;
	csg = create_csg(CSG_UNION, NULL, NULL);
	if (!csg)
		abort_rt_parse(parser, "Malloc failed for CSG (csg) object", MALLOC);
	append_object(csg, parser);
	parse_csg_headers(&ptr, csg, parser);
	position = parse_tuple(&ptr, &error, 1.0);
	normal = parse_normal(&ptr, &error);
	scale = atof_advance(&ptr, &error, false);
	if (error || scale <= 0.0)
		abort_rt_parse(parser, "Invalid CSG (csg) formatting", RT_PARSE);
	parse_object_tags(csg, ptr, parser);
	apply_universal_transform(csg, position, normal,
		vector(scale, scale, scale));
}
