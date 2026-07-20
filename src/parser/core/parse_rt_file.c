/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_rt_file.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 20:41:59 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/16 22:30:55 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "parser.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void init_rt_parse(t_rt_parser *parser, t_engine *engine)
 * @brief Initializes the master parsing state.
 * @details Zeroes out all tracking variables, resets the bitmask flags, and 
 * ensures the core boolean states for mandatory scene elements (Resolution, 
 * Camera, Ambient) start as false.
 * @param parser Pointer to the RT parsing state struct.
 * @param engine Pointer to the global engine context.
 */
static void	init_rt_parse(t_rt_parser *parser, t_engine *engine)
{
	parser->engine = engine;
	parser->file.fd = -1;
	parser->file.buf_pos = 0;
	parser->file.buf_bytes = 0;
	parser->tag_flags = 0;
	parser->objects = NULL;
	parser->has_resolution = false;
	parser->has_ambient = false;
	parser->has_camera = false;
}

/**
 * @fn static bool route_environment(char *id, const char *ptr, 
 * t_rt_parser *parser)
 * @brief Attempts to route a string identifier to an environment function.
 * @details Compares the extracted ID against an array of known environment 
 * tags (A, C, L, R). If a match is found, it executes the corresponding 
 * function pointer from the jump table.
 * @param id     The extracted string identifier.
 * @param ptr    Double pointer to the remaining arguments string.
 * @param parser Pointer to the active parsing state.
 * @return       True if routed successfully, false if the ID is not 
 * environmental.
 */
static bool	route_environment(char *id, const char *ptr, t_rt_parser *parser)
{
	static const t_parse_route	routes[] = {
	{"A", parse_ambient},
	{"C", parse_camera},
	{"L", parse_light},
	{"R", parse_resolution}
	};
	int							i;

	i = -1;
	while (++i < 4)
	{
		if (!ft_strncmp(id, routes[i].id, 2))
		{
			routes[i].parse_func(ptr, parser);
			return (true);
		}
	}
	return (false);
}

/**
 * @fn static bool route_geometry(char *id, const char *ptr, 
 * t_rt_parser *parser)
 * @brief Attempts to route a string identifier to a geometry function.
 * @details Compares the extracted ID against an array of known shape tags 
 * (sp, pl, cb, etc.). Executes the target function pointer if a match is found.
 * @param id     The extracted string identifier.
 * @param ptr    Double pointer to the remaining arguments string.
 * @param parser Pointer to the active parsing state.
 * @return       True if routed successfully, false if the ID is unrecognized.
 */
static bool	route_geometry(char *id, const char *ptr, t_rt_parser *parser)
{
	static const t_parse_route	routes[] = {
	{"sp", parse_sphere},
	{"pl", parse_plane},
	{"cb", parse_cube},
	{"cy", parse_cylinder},
	{"cn", parse_cone},
	{"to", parse_torus},
	{"tr", parse_triangle},
	{"smt", parse_smooth_triangle},
	{"obj", parse_obj},
	{"grp", parse_group},
	{"csg", parse_csg}
	};
	int							i;

	i = -1;
	while (++i < 11)
	{
		if (!ft_strncmp(id, routes[i].id, 4))
		{
			routes[i].parse_func(ptr, parser);
			return (true);
		}
	}
	return (false);
}

/**
 * @fn static void parse_rt_line(char *line, t_rt_parser *parser)
 * @brief Evaluates and processes a single line from the .rt file.
 * @details Bypasses empty lines and comments (#). Extracts the primary 
 * identifier block and delegates it to the routing tables. If both routers 
 * reject the identifier, it triggers an instant parsing abort.
 * @param line   The raw string line extracted from the file.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_rt_line(char *line, t_rt_parser *parser)
{
	const char	*ptr;
	char		id[5];
	int			i;

	parser->tag_flags = 0;
	ptr = line;
	while (ft_isspace(*ptr))
		ptr++;
	if (!*ptr || *ptr == '#' || *ptr == '\n')
		return ;
	i = -1;
	while (ptr[++i] && !ft_isspace(ptr[i]) && i < 4)
		id[i] = ptr[i];
	id[i] = '\0';
	ptr += i;
	if (!route_environment(id, ptr, parser) && !route_geometry(id, ptr, parser))
		abort_rt_parse(parser, "Unknown identifier in .rt file", RT_PARSE);
}

/**
 * @fn void parse_rt_file(char *filepath, t_engine *engine)
 * @brief Master controller for decoding the entire scene file.
 * @details Opens the requested .rt file and loops the fast-reading buffer to 
 * parse the scene. Once the file is fully read, it evaluates if all mandatory 
 * tags were found, links the parent/child hierarchy, and flattens the world 
 * for BVH optimization.
 * @param filepath The system path to the requested file.
 * @param engine   Pointer to the global engine context.
 */
void	parse_rt_file(char *filepath, t_engine *engine)
{
	t_rt_parser	parser;
	char		line[BUFFER_SIZE];

	init_rt_parse(&parser, engine);
	parser.file.fd = open(filepath, O_RDONLY);
	if (parser.file.fd < 0)
		abort_rt_parse(&parser, "Could not open .rt file", RT_OPEN);
	while (fast_read_line(line, &parser.file))
		parse_rt_line(line, &parser);
	close(parser.file.fd);
	if (!parser.has_ambient)
		abort_rt_parse(&parser, "No Ambient light (A) found in scene",
			RT_PARSE);
	if (!parser.has_camera)
		abort_rt_parse(&parser, "No Camera (C) found in scene", RT_PARSE);
	if (!parser.engine->world.lights)
		abort_rt_parse(&parser, "No Light source (L) found in scene", RT_PARSE);
	link_object_hierarchy(&parser);
	flatten_world(&engine->world, parser.objects, engine);
}
