/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_group.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 18:24:00 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/03 15:27:39 by maaugust         ###   ########.fr       */
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
 * @fn void parse_group(const char *ptr, t_rt_parser *parser)
 * @brief Extracts configuration data for a Group logical container.
 * @details Reads the position, orientation, and scale. Evaluates the trailing 
 * modifier tags specifically to enforce the presence of an `id:` tag. Without 
 * an ID, child objects cannot physically link themselves to the group.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_group(const char *ptr, t_rt_parser *parser)
{
	bool		error;
	t_tuple		position;
	t_tuple		normal;
	double		scale;
	t_object	*group;

	error = false;
	position = parse_tuple(&ptr, &error, 1.0);
	normal = parse_normal(&ptr, &error);
	scale = atof_advance(&ptr, &error, false);
	if (error || scale <= 0.0)
		abort_rt_parse(parser, "Invalid Group (grp) formatting", RT_PARSE);
	group = create_group();
	if (!group)
		abort_rt_parse(parser, "Malloc failed for group object", MALLOC);
	append_object(group, parser);
	parse_object_tags(group, ptr, parser);
	apply_universal_transform(group, position, normal,
		vector(scale, scale, scale));
	if (!(parser->tag_flags & TAG_ID))
		abort_rt_parse(parser, "Group (grp) is missing id tag", RT_PARSE);
}
