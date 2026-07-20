/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_transform_tags.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 13:51:40 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/14 05:53:25 by maaugust         ###   ########.fr       */
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
 * @fn static void parse_rot_tag(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Extracts local Euler angles (Pitch, Yaw, Roll) for the object.
 * @details Reads the X, Y, and Z rotation in degrees and converts them to 
 * radians for the engine's internal matrix math. Used to freely rotate 
 * textures or bypass the 2-degrees-of-freedom limit of normal vectors.
 * @param object Pointer to the geometric object.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_rot_tag(t_object *object, const char **ptr,
	t_rt_parser *parser)
{
	bool	error;
	t_tuple	rot;

	error = false;
	if (parser->tag_flags & TAG_ROT)
		abort_rt_parse(parser, "Duplicate object rot tag", RT_PARSE);
	parser->tag_flags |= TAG_ROT;
	rot.x = atof_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	rot.y = atof_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	rot.z = atof_advance(ptr, &error, false);
	if (error)
		abort_rt_parse(parser, "Invalid rot formatting", RT_PARSE);
	object->local_rotation.x = rot.x * (M_PI / 180.0);
	object->local_rotation.y = rot.y * (M_PI / 180.0);
	object->local_rotation.z = rot.z * (M_PI / 180.0);
}

/**
 * @fn static void parse_shr_tag(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Extracts the 6 proportional skew values for a Shearing transformation.
 * @details Parses the [xy, xz, yx, yz, zx, zy] parameters. Constructs the 
 * shearing matrix and securely caches it in the object struct alongside an 
 * active flag. Deferring the application guarantees it won't be overwritten 
 * during the universal transform generation.
 * @param object Pointer to the geometric object.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_shr_tag(t_object *object, const char **ptr,
	t_rt_parser *parser)
{
	bool	error;
	t_shear	shear;

	error = false;
	if (parser->tag_flags & TAG_SHR)
		abort_rt_parse(parser, "Duplicate object shr tag", RT_PARSE);
	parser->tag_flags |= TAG_SHR;
	shear.xy = atof_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	shear.xz = atof_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	shear.yx = atof_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	shear.yz = atof_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	shear.zx = atof_advance(ptr, &error, true);
	advance_comma(ptr, &error);
	shear.zy = atof_advance(ptr, &error, false);
	if (error)
		abort_rt_parse(parser, "Invalid shr formatting", RT_PARSE);
	object->has_shear = true;
	object->shear_matrix = shearing_matrix(&shear);
}

/**
 * @fn static void parse_mov_tag(t_object *object, const char **ptr, 
 * t_rt_parser *parser)
 * @brief Extracts the end-frame translation coordinate for Motion Blur.
 * @details Reads the desired destination coordinate. Sets the `is_moving` flag 
 * to true and alters the `transform_end` matrix to properly prepare the object 
 * for Matrix Linear Interpolation during the temporal ray loop.
 * @param object Pointer to the geometric object.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
static void	parse_mov_tag(t_object *object, const char **ptr,
	t_rt_parser *parser)
{
	bool	error;
	t_tuple	end_pos;

	error = false;
	if (parser->tag_flags & TAG_MOV)
		abort_rt_parse(parser, "Duplicate object mov tag", RT_PARSE);
	parser->tag_flags |= TAG_MOV;
	end_pos = parse_tuple(ptr, &error, 1.0);
	if (error)
		abort_rt_parse(parser, "Invalid mov formatting", RT_PARSE);
	object->is_moving = true;
	object->transform_end = object->transform;
	object->transform_end.m[0][3] = end_pos.x;
	object->transform_end.m[1][3] = end_pos.y;
	object->transform_end.m[2][3] = end_pos.z;
}

/**
 * @fn void parse_transform_tags(t_object *object, const char *key, 
 * const char **ptr, t_rt_parser *parser)
 * @brief Routes affine transformation tags to their sub-extractors.
 * @details Delegates rotation, shearing, and temporal motion blur offsets.
 * @param object Pointer to the geometric object.
 * @param key    The parsed modifier identifier string.
 * @param ptr    Double pointer to the raw string argument.
 * @param parser Pointer to the active parsing state.
 */
void	parse_transform_tags(t_object *object, const char *key,
	const char **ptr, t_rt_parser *parser)
{
	if (!ft_strncmp(key, "rot:", 4))
		parse_rot_tag(object, ptr, parser);
	else if (!ft_strncmp(key, "shr:", 4))
		parse_shr_tag(object, ptr, parser);
	else if (!ft_strncmp(key, "mov:", 4))
		parse_mov_tag(object, ptr, parser);
	else
		abort_rt_parse(parser, "Unknown transform tag bypassed router",
			RT_PARSE);
}
