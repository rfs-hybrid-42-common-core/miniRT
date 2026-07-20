/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_light.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 15:24:44 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 03:20:07 by maaugust         ###   ########.fr       */
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
 * @fn static void route_light_tag(t_light *light, const char *key, 
 * const char **ptr, t_rt_parser *parser)
 * @brief Evaluates an optional light modifier string block.
 * @details Intercepts explicitly supported tags for advanced light types 
 * (type, dir, cutoffs, grid steps, geometric vectors) and routes them.
 * @param light  Pointer to the temporary light state.
 * @param key    The parsed modifier identifier string.
 * @param ptr    Double pointer to the associated argument values.
 * @param parser Pointer to the active parsing state.
 */
static void	route_light_tag(t_light *light, const char *key,
	const char **ptr, t_rt_parser *parser)
{
	if (!ft_strncmp(key, "type:", 5))
		parse_light_tag_type(light, ptr, parser);
	else if (!ft_strncmp(key, "dir:", 4))
		parse_light_tag_direction(light, ptr, parser);
	else if (!ft_strncmp(key, "in:", 3) || !ft_strncmp(key, "out:", 4))
		parse_light_tag_cutoff(light, key, ptr, parser);
	else if (!ft_strncmp(key, "ustep:", 6) || !ft_strncmp(key, "vstep:", 6))
		parse_light_tag_steps(light, key, ptr, parser);
	else if (!ft_strncmp(key, "uvec:", 5) || !ft_strncmp(key, "vvec:", 5))
		parse_light_tag_vectors(light, key, ptr, parser);
	else
		abort_rt_parse(parser, "Unknown bonus tag on Light (L)", RT_PARSE);
}

/**
 * @fn static void parse_light_tags(t_light *light, const char *ptr, 
 * t_rt_parser *parser)
 * @brief Iterates through all trailing modifier tags for a light source.
 * @details Continuously isolates modifier keys (ending with ':') and delegates 
 * them to the routing function until hitting the end of the line or a comment.
 * @param light  Pointer to the temporary light state.
 * @param ptr    The string containing the remaining modifiers.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_light_tags(t_light *light, const char *ptr,
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
		route_light_tag(light, key, &ptr, parser);
	}
}

/**
 * @fn static void validate_light_logic(t_light *light, t_rt_parser *parser)
 * @brief Asserts that the provided tags logically match the specific light 
 * type.
 * @details Evaluates the parser's bitmask flag. Ensures Area lights don't 
 * receive Spot cutoff angles, ensures Point lights receive strictly no tags 
 * at all, and verifies that dependent parameters (inner vs outer cutoffs) 
 * mathematically make sense relative to one another.
 * @param light  Pointer to the temporary light state.
 * @param parser Pointer to the active parsing state holding the bitmask.
 */
static void	validate_light_logic(t_light *light, t_rt_parser *parser)
{
	if (light->type == LIGHT_POINT)
	{
		if (parser->tag_flags & ~TAG_TYPE)
			abort_rt_parse(parser, "Point Light has invalid tags", RT_PARSE);
	}
	else if (light->type == LIGHT_SPOT)
	{
		if (parser->tag_flags & (TAG_USTEP | TAG_VSTEP | TAG_UVEC | TAG_VVEC))
			abort_rt_parse(parser, "Spot Light has invalid tags", RT_PARSE);
		if (!(parser->tag_flags & TAG_DIR))
			abort_rt_parse(parser,
				"Spot Light missing direction tag", RT_PARSE);
		if (light->inner_cutoff > light->outer_cutoff)
			abort_rt_parse(parser,
				"Spot Light inner cutoff exceeds outer cutoff", RT_PARSE);
	}
	else if (light->type == LIGHT_AREA)
	{
		if (parser->tag_flags & (TAG_DIR | TAG_IN | TAG_OUT))
			abort_rt_parse(parser, "Area Light has invalid tags", RT_PARSE);
		if (!(parser->tag_flags & TAG_UVEC) || !(parser->tag_flags & TAG_VVEC))
			abort_rt_parse(parser, "Area Light missing vector tags", RT_PARSE);
	}
	else
		abort_rt_parse(parser, "Invalid Light (L) type value", RT_PARSE);
}

/**
 * @fn static t_light *build_final_light(t_light *tmp)
 * @brief Commits the verified temporary state into a dynamically allocated 
 * object.
 * @details Evaluates the finalized type of light and calls the specific memory 
 * allocator and initializer for it. Crucial for triggering the jitter sequence 
 * allocation specifically when Area Lights are requested.
 * @param tmp The stack-allocated temporary state struct.
 * @return    A pointer to the permanently allocated light struct.
 */
static t_light	*build_final_light(t_light *tmp)
{
	t_light	*light;
	t_tuple	full_uv[2];
	int		uvsteps[2];

	if (tmp->type == LIGHT_AREA)
	{
		full_uv[0] = tmp->uvec;
		full_uv[1] = tmp->vvec;
		uvsteps[0] = tmp->usteps;
		uvsteps[1] = tmp->vsteps;
		return (create_area_light(tmp->position, full_uv, uvsteps,
				tmp->intensity));
	}
	light = create_point_light(tmp->position, tmp->intensity);
	if (light && tmp->type == LIGHT_SPOT)
	{
		light->type = LIGHT_SPOT;
		light->direction = tmp->direction;
		light->inner_cutoff = tmp->inner_cutoff;
		light->outer_cutoff = tmp->outer_cutoff;
	}
	return (light);
}

/**
 * @fn void parse_light(const char *ptr, t_rt_parser *parser)
 * @brief Master extraction function for Light ('L') elements.
 * @details Unlike ambient/camera tags, lights do not restrict instantiation 
 * limits (multiple lights allowed). Generates a temporary state struct on the 
 * stack to load default values (like default cutoffs). Evaluates tags, 
 * converts degrees to cosine angles for math optimization, and explicitly adds 
 * the final light to the global scene list.
 * @param ptr    Double pointer to the raw string block.
 * @param parser Pointer to the active parsing state.
 */
void	parse_light(const char *ptr, t_rt_parser *parser)
{
	bool	error;
	double	ratio;
	t_light	*light;
	t_light	tmp;

	error = false;
	ft_bzero(&tmp, sizeof(t_light));
	tmp.type = LIGHT_POINT;
	tmp.position = parse_tuple(&ptr, &error, 1.0);
	ratio = atof_advance(&ptr, &error, false);
	tmp.intensity = scaling_color(parse_color(&ptr, &error), ratio);
	if (error || ratio < 0.0 || ratio > 1.0)
		abort_rt_parse(parser, "Invalid light (L) formatting", RT_PARSE);
	tmp.inner_cutoff = LIGHT_IN_CUT_DEFAULT;
	tmp.outer_cutoff = LIGHT_OUT_CUT_DEFAULT;
	tmp.usteps = LIGHT_USTEPS_DEFAULT;
	tmp.vsteps = LIGHT_VSTEPS_DEFAULT;
	parse_light_tags(&tmp, ptr, parser);
	validate_light_logic(&tmp, parser);
	tmp.inner_cutoff = cos(tmp.inner_cutoff * (M_PI / 180.0));
	tmp.outer_cutoff = cos(tmp.outer_cutoff * (M_PI / 180.0));
	light = build_final_light(&tmp);
	append_light(light, parser);
}
