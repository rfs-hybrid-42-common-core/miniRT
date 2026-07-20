/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safe_list_append.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 21:02:16 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 03:09:52 by maaugust         ###   ########.fr       */
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
 * @fn void append_light(t_light *light, t_rt_parser *parser)
 * @brief Wraps a successful light object into a linked list node.
 * @details If either the primary allocation or the node allocation fails, it 
 * safely cleans up any nested dynamic arrays (like the Area Light Jitter 
 * Array) before throwing an abort_rt_parse to prevent orphaned memory.
 * @param light  The fully configured light to link.
 * @param parser Pointer to the global parsing context.
 */
void	append_light(t_light *light, t_rt_parser *parser)
{
	t_list	*new_node;

	if (!light)
		abort_rt_parse(parser, "Malloc failed for light struct", MALLOC);
	new_node = ft_lstnew(light);
	if (!new_node)
	{
		if (light->jitter)
			free(light->jitter);
		free(light);
		abort_rt_parse(parser, "Malloc failed for light list node", MALLOC);
	}
	ft_lstadd_back(&parser->engine->world.lights, new_node);
}

/**
 * @fn void append_object(t_object *object, t_rt_parser *parser)
 * @brief Wraps a fully validated primitive object into a linked list node.
 * @details Enacts complete failure safeguarding. If the node allocation fails, 
 * it triggers the master `free_object` function to safely crawl the object 
 * and destroy any internal loaded XPMs or dynamically constructed nested 
 * structures (like child groups).
 * @param object The fully parsed renderable primitive.
 * @param parser Pointer to the global parsing context.
 */
void	append_object(t_object *object, t_rt_parser *parser)
{
	t_list	*new_node;

	if (!object)
		abort_rt_parse(parser, "Malloc failed for object struct", MALLOC);
	new_node = ft_lstnew(object);
	if (!new_node)
	{
		free_object(object, parser->engine->mlx.mlx_ptr);
		abort_rt_parse(parser, "Malloc failed for object list node", MALLOC);
	}
	ft_lstadd_back(&parser->objects, new_node);
}
