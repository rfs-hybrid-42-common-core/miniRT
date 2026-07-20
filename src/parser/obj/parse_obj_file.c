/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_obj_file.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 15:38:37 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 03:54:06 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"
#include "parser.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <fcntl.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void init_obj_parse(t_obj_parser *obj_parser, 
 * t_rt_parser *rt_parser)
 * @brief Initializes the two-pass OBJ parsing state.
 * @details Provisions the master group that will hold the mesh. Sets all array 
 * tracking variables to zero. Note that OBJ indices are 1-based rather than 
 * 0-based, so the tracking indices start at 1.
 * @param obj_parser Pointer to the internal OBJ parsing state.
 * @param rt_parser  Pointer to the global parent RT parsing state.
 */
static void	init_obj_parse(t_obj_parser *obj_parser, t_rt_parser *rt_parser)
{
	obj_parser->rt_parser = rt_parser;
	obj_parser->file.fd = -1;
	obj_parser->file.buf_pos = 0;
	obj_parser->file.buf_bytes = 0;
	obj_parser->main_group = create_group();
	if (!obj_parser->main_group)
		abort_obj_parse(obj_parser,
			"Memory allocation failed for OBJ main group", NULL, MALLOC);
	obj_parser->current_group = obj_parser->main_group;
	obj_parser->v_array = NULL;
	obj_parser->vt_array = NULL;
	obj_parser->vn_array = NULL;
	obj_parser->v_count = 0;
	obj_parser->v_index = 1;
	obj_parser->vt_count = 0;
	obj_parser->vt_index = 1;
	obj_parser->vn_count = 0;
	obj_parser->vn_index = 1;
}

/**
 * @fn static void count_obj_line(char *line, t_obj_parser *parser)
 * @brief Pass 1 Route: Counts parameter tags to establish array sizes.
 * @details Identifies 'v' (vertex), 'vt' (UV), and 'vn' (normal) lines and 
 * increments their respective counters.
 * @param line   The string line extracted from the OBJ file.
 * @param parser Pointer to the OBJ parsing state.
 */
static void	count_obj_line(char *line, t_obj_parser *parser)
{
	char	*ptr;

	ptr = line;
	while (ft_isspace(*ptr))
		ptr++;
	if (!*ptr || *ptr == '#' || *ptr == '\n')
		return ;
	if (ptr[0] == 'v' && ft_isspace(ptr[1]))
		parser->v_count++;
	else if (ptr[0] == 'v' && ptr[1] == 't' && ft_isspace(ptr[2]))
		parser->vt_count++;
	else if (ptr[0] == 'v' && ptr[1] == 'n' && ft_isspace(ptr[2]))
		parser->vn_count++;
}

/**
 * @fn static void allocate_arrays(t_obj_parser *parser)
 * @brief Physically allocates the contiguous flat arrays for mesh data.
 * @details Using flat arrays provides O(1) instantaneous lookup speeds when 
 * generating triangles, which is millions of times faster than traversing a 
 * linked list. Allocates Count + 1 to account for 1-based OBJ indexing.
 * @param parser Pointer to the OBJ parsing state.
 */
static void	allocate_arrays(t_obj_parser *parser)
{
	if (parser->v_count > 0)
		parser->v_array = malloc(sizeof(t_tuple) * (parser->v_count + 1));
	if (parser->vt_count > 0)
		parser->vt_array = malloc(sizeof(t_tuple) * (parser->vt_count + 1));
	if (parser->vn_count > 0)
		parser->vn_array = malloc(sizeof(t_tuple) * (parser->vn_count + 1));
	if ((parser->v_count > 0 && !parser->v_array)
		|| (parser->vt_count > 0 && !parser->vt_array)
		|| (parser->vn_count > 0 && !parser->vn_array))
		abort_obj_parse(parser, "Memory allocation failed for OBJ arrays",
			NULL, MALLOC);
}

/**
 * @fn static void route_obj_line(char *line, t_obj_parser *parser)
 * @brief Pass 2 Route: Parses the data and constructs the geometry.
 * @details Evaluates the line identifiers and hands them off to the explicit 
 * extraction handlers for parameters (v, vt, vn) and geometry (f, g).
 * @param line   The string line extracted from the OBJ file.
 * @param parser Pointer to the OBJ parsing state.
 */
static void	route_obj_line(char *line, t_obj_parser *parser)
{
	char	*ptr;

	ptr = line;
	while (ft_isspace(*ptr))
		ptr++;
	if (!*ptr || *ptr == '#' || *ptr == '\n')
		return ;
	if (ptr[0] == 'v' && ft_isspace(ptr[1]))
		parse_v(ptr, parser);
	else if (ptr[0] == 'v' && ptr[1] == 't' && ft_isspace(ptr[2]))
		parse_vt(ptr, parser);
	else if (ptr[0] == 'v' && ptr[1] == 'n' && ft_isspace(ptr[2]))
		parse_vn(ptr, parser);
	else if (ptr[0] == 'f' && ft_isspace(ptr[1]))
		parse_f(ptr, parser);
	else if (ptr[0] == 'g' && (ft_isspace(ptr[1]) || !ptr[1]))
		parse_g(ptr, parser);
}

/**
 * @fn t_object *parse_obj_file(char *filepath, t_rt_parser *rt_parser)
 * @brief Master routine for the Two-Pass OBJ parsing architecture.
 * @details First pass: Opens the file, rapidly counts all the geometric tags, 
 * closes the file, and allocates the required memory. Second pass: Re-opens 
 * the file, extracts all coordinates into the arrays, constructs the 
 * triangles, closes the file, and finally frees the temporary parameter arrays.
 * @param filepath  The system path to the .obj file.
 * @param rt_parser Pointer to the global parent RT parsing state.
 * @return          Pointer to the root group containing the finished mesh.
 */
t_object	*parse_obj_file(char *filepath, t_rt_parser *rt_parser)
{
	t_obj_parser	obj_parser;
	char			line[BUFFER_SIZE];

	init_obj_parse(&obj_parser, rt_parser);
	obj_parser.file.fd = open(filepath, O_RDONLY);
	if (obj_parser.file.fd < 0)
		abort_obj_parse(&obj_parser, "Could not open OBJ file", filepath,
			OBJ_OPEN);
	while (fast_read_line(line, &obj_parser.file))
		count_obj_line(line, &obj_parser);
	close(obj_parser.file.fd);
	allocate_arrays(&obj_parser);
	obj_parser.file.fd = open(filepath, O_RDONLY);
	if (obj_parser.file.fd < 0)
		abort_obj_parse(&obj_parser, "Could not open OBJ file", filepath,
			OBJ_OPEN);
	obj_parser.file.buf_pos = 0;
	obj_parser.file.buf_bytes = 0;
	while (fast_read_line(line, &obj_parser.file))
		route_obj_line(line, &obj_parser);
	close(obj_parser.file.fd);
	free(obj_parser.v_array);
	free(obj_parser.vt_array);
	free(obj_parser.vn_array);
	return (obj_parser.main_group);
}
