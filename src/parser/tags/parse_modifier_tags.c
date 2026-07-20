/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_modifier_tags.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 19:13:10 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/15 02:38:31 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

#include "libft.h"
#include "parser.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void parse_caps_tag(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Toggles geometric capping for specific truncated primitives.
 * @details Reads a binary 0 or 1 value. Aborts if this tag is arbitrarily 
 * applied to infinite or solid shapes (like spheres or planes) where capping 
 * makes no geometric sense.
 * @param object Pointer to the geometric object.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_caps_tag(t_object *object, const char **ptr,
	t_rt_parser *parser)
{
	bool	error;
	int		val;

	error = false;
	if (parser->tag_flags & TAG_CAPS)
		abort_rt_parse(parser, "Duplicate object caps tag", RT_PARSE);
	parser->tag_flags |= TAG_CAPS;
	if (object->type != CYLINDER && object->type != CONE)
		abort_rt_parse(parser, "Caps tag applied to unsupported geometry",
			RT_PARSE);
	val = atoi_advance(ptr, &error, false);
	if (error || (val != 0 && val != 1))
		abort_rt_parse(parser, "Invalid caps formatting (expected 0 or 1)",
			RT_PARSE);
	if (object->type == CYLINDER)
		object->data.cylinder.closed = (val == 1);
	else
		object->data.cone.closed = (val == 1);
}

/**
 * @fn static void parse_string_tag(char **dest, const char *key, 
 * const char **ptr, t_rt_parser *parser)
 * @brief Extracts an alphanumeric string identifier literal.
 * @details Used to capture 'id:' names and 'parent:' linker requirements for 
 * assembling CSG and Group hierarchy trees in the final parsing step.
 * @param dest   Pointer to the string attribute being modified.
 * @param key    The parsed modifier identifier string.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_string_tag(char **dest, const char *key,
	const char **ptr, t_rt_parser *parser)
{
	char	buffer[MAX_MODIFIER_LEN];
	int		bit;
	int		i;

	bit = 0;
	if (!ft_strncmp(key, "id:", 3))
		bit = TAG_ID;
	else if (!ft_strncmp(key, "parent:", 7))
		bit = TAG_PARENT;
	else
		abort_rt_parse(parser, "Unknown modifier tag bypassed router",
			RT_PARSE);
	if (parser->tag_flags & bit)
		abort_rt_parse(parser, "Duplicate object id or parent tag", RT_PARSE);
	parser->tag_flags |= bit;
	i = 0;
	while (**ptr && !ft_isspace(**ptr) && i < MAX_MODIFIER_LEN - 1)
		buffer[i++] = *(*ptr)++;
	buffer[i] = '\0';
	if (i == 0)
		abort_rt_parse(parser, "Empty string provided for id or parent",
			RT_PARSE);
	*dest = ft_strdup(buffer);
	if (!*dest)
		abort_rt_parse(parser, "Malloc failed for id or parent string", MALLOC);
}

/**
 * @fn void parse_modifier_tags(t_object *object, const char *key, 
 * const char **ptr, t_rt_parser *parser)
 * @brief Routes specific logic control tags to their sub-extractors.
 * @details Delegates structural caps, string identities, and parent grouping 
 * links for CSG/Group hierarchy assembly.
 * @param object Pointer to the geometric object.
 * @param key    The parsed modifier identifier string.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_modifier_tags(t_object *object, const char *key,
	const char **ptr, t_rt_parser *parser)
{
	if (!ft_strncmp(key, "caps:", 5))
		parse_caps_tag(object, ptr, parser);
	else if (!ft_strncmp(key, "id:", 3))
		parse_string_tag(&object->id, key, ptr, parser);
	else if (!ft_strncmp(key, "parent:", 7))
		parse_string_tag(&object->parent_id, key, ptr, parser);
	else
		abort_rt_parse(parser, "Unknown modifier tag bypassed router",
			RT_PARSE);
}
