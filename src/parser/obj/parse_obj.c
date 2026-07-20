/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_obj.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/26 02:36:56 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/14 04:39:19 by maaugust         ###   ########.fr       */
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
 * @fn static t_object *load_obj_file(const char **ptr, bool *error, 
 * t_rt_parser *parser)
 * @brief Extracts the file path and hands off execution to the OBJ sub-parser.
 * @details Isolates the .obj filename string. Triggers an abort if the path 
 * string is empty.
 * @param ptr    Double pointer to the raw string argument.
 * @param error  Pointer to the global error flag.
 * @param parser Pointer to the active parsing state.
 * @return       Pointer to the root group containing the fully constructed 
 * mesh.
 */
static t_object	*load_obj_file(const char **ptr, bool *error,
	t_rt_parser *parser)
{
	char	filepath[MAX_PATH_LEN];

	extract_token(ptr, filepath, MAX_PATH_LEN);
	if (!filepath[0])
	{
		*error = true;
		return (NULL);
	}
	return (parse_obj_file(filepath, parser));
}

/**
 * @fn static void paint_obj_group(t_object *group, t_material *material)
 * @brief Recursively applies the base material to all triangles in the mesh.
 * @details Because the .obj file defines pure geometry, the parent .rt file 
 * defines the color/material. This transverses the nested sub-groups to ensure 
 * every individual triangle primitive inherits the master material properties.
 * @param group    Pointer to the root or sub-group being painted.
 * @param material Pointer to the finalized material struct to inherit.
 */
static void	paint_obj_group(t_object *group, t_material *material)
{
	t_list		*current;
	t_object	*child;

	if (!group || group->type != GROUP)
		return ;
	current = group->data.group.children;
	while (current)
	{
		child = (t_object *)current->content;
		if (child->type == GROUP)
			paint_obj_group(child, material);
		else
			child->material = *material;
		current = current->next;
	}
}

/**
 * @fn void parse_obj(const char *ptr, t_rt_parser *parser)
 * @brief Master extraction function for External Mesh ('obj') elements.
 * @details Reads the position, normal orientation, scale, and base color from 
 * the .rt file. Suspends the main parser to launch the `parse_obj_file` 
 * routine. Once the mesh returns as a unified group, it applies the 
 * transformations and physically paints the triangles before appending it to 
 * the scene.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_obj(const char *ptr, t_rt_parser *parser)
{
	bool		error;
	t_tuple		pos_norm[2];
	double		scale;
	t_color		rgb;
	t_object	*obj_group;

	error = false;
	pos_norm[0] = parse_tuple(&ptr, &error, 1.0);
	pos_norm[1] = parse_normal(&ptr, &error);
	scale = atof_advance(&ptr, &error, false);
	rgb = parse_color(&ptr, &error);
	obj_group = load_obj_file(&ptr, &error, parser);
	if (error || scale <= 0.0 || !obj_group)
		abort_rt_parse(parser, "Invalid OBJ format or load failure", RT_PARSE);
	obj_group->material.color = rgb;
	append_object(obj_group, parser);
	parse_object_tags(obj_group, ptr, parser);
	apply_universal_transform(obj_group, pos_norm[0], pos_norm[1],
		vector(scale, scale, scale));
	paint_obj_group(obj_group, &obj_group->material);
}
