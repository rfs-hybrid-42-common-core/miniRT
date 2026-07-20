/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_linker.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 20:53:44 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/14 00:15:14 by maaugust         ###   ########.fr       */
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

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static t_object *find_object_by_id(t_list *objects, char *id)
 * @brief Scans the flat world list for a specific string identifier.
 * @details Used by the linker to resolve dependency relations declared by 
 * `parent:` tags and CSG left/right assignments.
 * @param objects The flat linked list containing all parsed geometry.
 * @param id      The specific string identifier to search for.
 * @return        Pointer to the matched object, or NULL if missing.
 */
static t_object	*find_object_by_id(t_list *objects, char *id)
{
	t_object	*current;

	while (objects)
	{
		current = (t_object *)objects->content;
		if (current->id && !ft_strncmp(current->id, id, MAX_MODIFIER_LEN))
			return (current);
		objects = objects->next;
	}
	return (NULL);
}

/**
 * @fn static bool is_circular_dependency(t_object *node, t_object *target)
 * @brief Climbs a hierarchy branch to detect infinite loops.
 * @details Prevents an object from becoming its own parent, or forming a 
 * circular dependency (e.g., A is parent of B, B is parent of A).
 * @param node   The object attempting to link to a parent.
 * @param target The requested parent node.
 * @return       True if linking would cause an infinite loop, false otherwise.
 */
static bool	is_circular_dependency(t_object *node, t_object *target)
{
	t_object	*current;

	current = target;
	while (current)
	{
		if (current == node)
			return (true);
		current = current->parent;
	}
	return (false);
}

/**
 * @fn static void link_csg_children(t_object *csg, t_list *objects, 
 * t_rt_parser *parser)
 * @brief Resolves and binds the target geometries to a CSG boolean node.
 * @details Maps the requested left and right string IDs to their physical 
 * object pointers. Performs atomic validation to ensure the targets exist, 
 * are distinct entities, possess no existing parent, and form a strictly 
 * acyclic graph structure before finalizing the hierarchy links.
 * @param csg     Pointer to the active CSG logical node.
 * @param objects The flat list of parsed objects.
 * @param parser  Pointer to the active parsing state.
 */
static void	link_csg_children(t_object *csg, t_list *objects,
	t_rt_parser *parser)
{
	t_object	*left_csg;
	t_object	*right_csg;

	left_csg = find_object_by_id(objects, csg->data.csg.left_id);
	right_csg = find_object_by_id(objects, csg->data.csg.right_id);
	if (!left_csg || !right_csg)
		abort_rt_parse(parser, "CSG child ID not found in scene", RT_PARSE);
	if (left_csg == right_csg)
		abort_rt_parse(parser, "CSG left and right cannot be the same object",
			RT_PARSE);
	if (left_csg->parent || right_csg->parent)
		abort_rt_parse(parser, "CSG objects has multiple parents", RT_PARSE);
	if (is_circular_dependency(csg, left_csg)
		|| is_circular_dependency(csg, right_csg))
		abort_rt_parse(parser, "Circular CSG dependency", RT_PARSE);
	csg->data.csg.left = left_csg;
	csg->data.csg.right = right_csg;
	left_csg->parent = csg;
	right_csg->parent = csg;
}

/**
 * @fn static void build_hierarchy_tree(t_rt_parser *parser)
 * @brief Assembles the definitive object hierarchy from the flat parsed list.
 * @details Iterates post-parse to resolve explicit `parent:` tags and CSG 
 * relationships, allowing objects to reference targets declared further down 
 * the file. Validates the structural integrity of the scene by ensuring 
 * singular parentage and preventing circular lineage constraints before 
 * committing to the structural binds.
 * @param parser Pointer to the active parsing state.
 */
static void	build_hierarchy_tree(t_rt_parser *parser)
{
	t_list		*current;
	t_object	*object;
	t_object	*parent;

	current = parser->objects;
	while (current)
	{
		object = (t_object *)current->content;
		if (object->parent_id)
		{
			if (object->parent != NULL)
				abort_rt_parse(parser, "Object has multiple parents", RT_PARSE);
			parent = find_object_by_id(parser->objects, object->parent_id);
			if (!parent || parent->type != GROUP)
				abort_rt_parse(parser, "Invalid or missing parent group ID",
					RT_PARSE);
			if (is_circular_dependency(object, parent))
				abort_rt_parse(parser, "Circular group dependency", RT_PARSE);
			add_child(parent, object);
			object->parent = parent;
		}
		if (object->type == CSG)
			link_csg_children(object, parser->objects, parser);
		current = current->next;
	}
}

/**
 * @fn void link_object_hierarchy(t_rt_parser *parser)
 * @brief Master linkage controller to assemble the final rendering graph.
 * @details Resolves the parent/child tree, and then iterates through the flat 
 * list one final time. Any object that successfully linked to a parent is 
 * removed from the master list (since it is now encapsulated inside its 
 * parent's array). This guarantees the final list only contains pure "root" 
 * nodes, massively optimizing the final BVH construction.
 * @param parser Pointer to the active parsing state.
 */
void	link_object_hierarchy(t_rt_parser *parser)
{
	t_list		*current;
	t_list		*next;
	t_list		*root_list;
	t_object	*object;

	build_hierarchy_tree(parser);
	root_list = NULL;
	current = parser->objects;
	while (current)
	{
		object = (t_object *)current->content;
		next = current->next;
		if (object->parent == NULL)
		{
			current->next = NULL;
			ft_lstadd_back(&root_list, current);
		}
		else
			free(current);
		current = next;
	}
	parser->objects = root_list;
}
