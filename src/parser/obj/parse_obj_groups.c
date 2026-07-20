/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_obj_groups.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 15:02:56 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 03:55:08 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "parser.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stdlib.h>
#include <stddef.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn void parse_g(char *line, t_obj_parser *parser)
 * @brief Establishes a new organizational sub-group inside the mesh.
 * @details In complex meshes, different components (e.g., 'Tire', 'Chassis') 
 * are separated by 'g' tags. This spawns a new logical group, connects it to 
 * the root mesh group, and redirects all subsequent triangle constructions 
 * into it.
 * @param line   The string line starting with 'g'.
 * @param parser Pointer to the OBJ parsing state.
 */
void	parse_g(char *line, t_obj_parser *parser)
{
	t_object	*new_group;

	(void)line;
	new_group = create_group();
	if (!new_group)
		abort_obj_parse(parser, "Memory allocation failed for OBJ subgroup",
			NULL, MALLOC);
	add_child(parser->main_group, new_group);
	parser->current_group = new_group;
}
