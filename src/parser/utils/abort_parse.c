/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   abort_parse.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/21 19:25:39 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/14 00:14:55 by maaugust         ###   ########.fr       */
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
#include <unistd.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void free_parsed_objects(t_list *objects, void *mlx_ptr)
 * @brief Two-pass cleanup sweep for partially parsed objects.
 * @details Executes a hierarchical memory deallocation. It first isolates 
 * child elements that are managed by parent containers (like Groups or CSG 
 * nodes). It then performs a deep, cascading free on the remaining root 
 * geometries and their associated texture data, ensuring the entire parse 
 * tree is cleanly purged.
 * @param objects The flat linked list of objects to free.
 * @param mlx_ptr The active MiniLibX instance required to free textures.
 */
static void	free_parsed_objects(t_list *objects, void *mlx_ptr)
{
	t_list		*current;
	t_list		*next;
	t_object	*object;

	current = objects;
	while (current)
	{
		object = (t_object *)current->content;
		if (object && object->parent != NULL)
			current->content = NULL;
		current = current->next;
	}
	current = objects;
	while (current)
	{
		next = current->next;
		object = (t_object *)current->content;
		if (object)
			free_object(object, mlx_ptr);
		free(current);
		current = next;
	}
}

/**
 * @fn void abort_rt_parse(t_rt_parser *parser, char *error_msg, 
 * t_exit_code exit_code)
 * @brief Gracefully handles syntax/memory errors during .rt parsing.
 * @details Closes active file descriptors, delegates object cleanup, and 
 * triggers the engine's master exit routine.
 * @param parser    Pointer to the active parsing state.
 * @param error_msg The specific literal string explaining the error.
 * @param exit_code The enum code mapped to the engine's termination logic.
 */
void	abort_rt_parse(t_rt_parser *parser, char *error_msg,
	t_exit_code exit_code)
{
	if (error_msg)
		print_error(error_msg, NULL);
	if (parser->file.fd >= 0)
		close(parser->file.fd);
	free_parsed_objects(parser->objects, parser->engine->mlx.mlx_ptr);
	parser->engine->exit_code = exit_code;
	exit_program(parser->engine);
}

/**
 * @fn void abort_obj_parse(t_obj_parser *parser, char *error_msg, 
 * char *filepath, t_exit_code exit_code)
 * @brief Gracefully handles syntax/memory errors specifically within .obj 
 * meshes.
 * @details Cleans up the dynamically allocated massive data arrays (vertices, 
 * uvs, normals) that are unique to the OBJ multi-pass loading architecture. 
 * Closes the local .obj file descriptor before passing control back to the 
 * parent abort_rt_parse function.
 * @param parser    Pointer to the active OBJ parsing state.
 * @param error_msg The specific literal string explaining the error.
 * @param filepath  The path to the broken .obj file being printed in the log.
 * @param exit_code The enum code mapped to the engine's termination logic.
 */
void	abort_obj_parse(t_obj_parser *parser, char *error_msg, char *filepath,
	t_exit_code exit_code)
{
	print_error(error_msg, filepath);
	if (parser->v_array)
		free(parser->v_array);
	if (parser->vt_array)
		free(parser->vt_array);
	if (parser->vn_array)
		free(parser->vn_array);
	if (parser->main_group)
		free_object(parser->main_group, parser->rt_parser->engine->mlx.mlx_ptr);
	if (parser->file.fd >= 0)
		close(parser->file.fd);
	abort_rt_parse(parser->rt_parser, NULL, exit_code);
}
