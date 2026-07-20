/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_object_tags.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 18:36:34 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/03 13:54:36 by maaugust         ###   ########.fr       */
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
 * @fn static void route_object_tag(t_object *object, const char *key, 
 * const char **ptr, t_rt_parser *parser)
 * @brief Routes an extracted modifier key to its specific parsing sub-system.
 * @details Evaluates the isolated string identifier (e.g., "mat:", "glass:") 
 * and categorizes it into Material, Procedural, Texture, Transform, Modifier, 
 * or Environment groupings.
 * @param object Pointer to the geometric object receiving the modifier.
 * @param key    The extracted modifier identifier string.
 * @param ptr    Double pointer to the associated argument values.
 * @param parser Pointer to the active parsing state.
 */
static void	route_object_tag(t_object *object, const char *key,
	const char **ptr, t_rt_parser *parser)
{
	if (!ft_strncmp(key, "mat:", 4) || !ft_strncmp(key, "ref:", 4)
		|| !ft_strncmp(key, "glass:", 6))
		parse_material_tags(object, key, ptr, parser);
	else if (!ft_strncmp(key, "str:", 4) || !ft_strncmp(key, "gra:", 4)
		|| !ft_strncmp(key, "rng:", 4) || !ft_strncmp(key, "chk:", 4)
		|| !ft_strncmp(key, "marble:", 7) || !ft_strncmp(key, "perlin:", 7))
		parse_procedural_tags(object, key, ptr, parser);
	else if (!ft_strncmp(key, "uv_chk:", 7) || !ft_strncmp(key, "img:", 4)
		|| !ft_strncmp(key, "bump:", 5))
		parse_texture_tags(object, key, ptr, parser);
	else if (!ft_strncmp(key, "rot:", 4) || !ft_strncmp(key, "shr:", 4)
		|| !ft_strncmp(key, "mov:", 4))
		parse_transform_tags(object, key, ptr, parser);
	else if (!ft_strncmp(key, "caps:", 5) || !ft_strncmp(key, "id:", 3)
		|| !ft_strncmp(key, "parent:", 7))
		parse_modifier_tags(object, key, ptr, parser);
	else if (!ft_strncmp(key, "dome:", 5) || !ft_strncmp(key, "skybox:", 7))
		parse_environment_tags(object, key, ptr, parser);
	else
		abort_rt_parse(parser, "Unknown bonus tag on Geometric Object",
			RT_PARSE);
}

/**
 * @fn void parse_object_tags(t_object *object, const char *ptr, 
 * t_rt_parser *parser)
 * @brief Iterates through all trailing modifier tags for an object.
 * @details Resets the parser's bitmask flag tracker, then scans the remainder 
 * of the line. It isolates alphanumeric modifier keys ending with a colon 
 * (':'), advances the pointer past the key, and delegates the raw value 
 * extraction to the router.
 * @param object Pointer to the geometric object being modified.
 * @param ptr    The string containing the remaining modifiers.
 * @param parser Pointer to the active parsing state.
 */
void	parse_object_tags(t_object *object, const char *ptr,
	t_rt_parser *parser)
{
	char	key[MAX_TAG_LEN];
	int		i;

	parser->tag_flags = 0;
	while (*ptr)
	{
		while (ft_isspace(*ptr))
			ptr++;
		if (!*ptr || *ptr == '\n' || *ptr == '#')
			break ;
		i = 0;
		while (ptr[i] && !ft_isspace(ptr[i]) && ptr[i] != ':'
			&& i < MAX_TAG_LEN - 2)
			i++;
		if (ptr[i] == ':')
			i++;
		ft_strlcpy(key, ptr, i + 1);
		ptr += i;
		route_object_tag(object, key, &ptr, parser);
	}
}
