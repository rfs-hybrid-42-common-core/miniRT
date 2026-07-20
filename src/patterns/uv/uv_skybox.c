/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uv_skybox.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 00:27:23 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:45:06 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "mlx.h"
#include "patterns.h"

/* --------------------------- External Libraries --------------------------- */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void free_skybox_faces(t_pattern *pattern, void *mlx_ptr, 
 * int index)
 * @brief Cleans up incomplete skybox allocations.
 * @details Executed if one of the 6 texture files is missing. Frees the valid 
 * ones and instantly downgrades the material to a flat Magenta/Black UV 
 * checker to act as a highly visible "missing texture" warning during 
 * rendering.
 * @param pattern The active skybox pattern.
 * @param mlx_ptr The active MiniLibX instance.
 * @param index   The index of the face that triggered the failure.
 */
static void	free_skybox_faces(t_pattern *pattern, void *mlx_ptr, int index)
{
	int	i;

	i = -1;
	while (++i <= index)
	{
		if (pattern->u_data.s_skybox.faces[i])
		{
			if (pattern->u_data.s_skybox.faces[i]->img_ptr)
				mlx_destroy_image(mlx_ptr,
					pattern->u_data.s_skybox.faces[i]->img_ptr);
			free(pattern->u_data.s_skybox.faces[i]);
			pattern->u_data.s_skybox.faces[i] = NULL;
		}
	}
	pattern->type = UV_CHECKER;
	pattern->color_a = color(1, 0, 1);
	pattern->color_b = color(0, 0, 0);
	pattern->u_data.s_uv.width = 16;
	pattern->u_data.s_uv.height = 16;
}

/**
 * @fn t_pattern uv_skybox_pattern(void *mlx_ptr, char **filepaths)
 * @brief Loads a 6-sided environmental texture array.
 * @details Loads the right, left, up, down, front, and back XPM files. Only 
 * functions successfully if mapped onto an infinite bounding Cube object.
 * @param mlx_ptr   The active MiniLibX instance.
 * @param filepaths Array of the 6 explicit XPM paths.
 * @return          The configured pattern struct.
 */
t_pattern	uv_skybox_pattern(void *mlx_ptr, char **filepaths)
{
	t_pattern	pattern;
	t_texture	*texture;
	int			i;

	pattern.type = UV_SKYBOX;
	pattern.transform = identity_matrix();
	pattern.transform_inverse = pattern.transform;
	i = -1;
	while (++i < 6)
	{
		texture = malloc(sizeof(t_texture));
		pattern.u_data.s_skybox.faces[i] = texture;
		if (texture)
			texture->img_ptr = mlx_xpm_file_to_image(mlx_ptr, filepaths[i],
					&texture->width, &texture->height);
		if (!texture || !texture->img_ptr)
		{
			printf("Warning: Could not load texture '%s'\n", filepaths[i]);
			free_skybox_faces(&pattern, mlx_ptr, i);
			return (pattern);
		}
		texture->pixels = mlx_get_data_addr(texture->img_ptr, &texture->bpp,
				&texture->line_length, &texture->endian);
	}
	return (pattern);
}

/**
 * @fn static t_color uv_pattern_at(t_pattern *pattern, t_uv_coord uv_coord)
 * @brief Retrieves the color from the correct cubical face buffer.
 * @details Identifies which of the 6 texture arrays to index based on the 
 * face enumeration, then samples using rounding (nearest-neighbor) for 
 * skyboxes due to their typically massive resolution.
 * @param pattern  The skybox pattern.
 * @param uv_coord The cubical map results containing the target face.
 * @return         The pixel color.
 */
static t_color	uv_pattern_at(t_pattern *pattern, t_uv_coord uv_coord)
{
	t_texture	*texture;
	int			x;
	int			y;

	texture = pattern->u_data.s_skybox.faces[uv_coord.cube_face];
	x = (int)round(uv_coord.u * (texture->width - 1));
	y = (int)round((1.0 - uv_coord.v) * (texture->height - 1));
	return (get_pixel_color(texture, x, y));
}

/**
 * @fn t_color uv_skybox_at(t_pattern *pattern, t_tuple pattern_point)
 * @brief Evaluates an environmental skybox.
 * @details Forces a cubical wrapping algorithm and fetches the color from 
 * the appropriate 2D face buffer.
 * @param pattern       The skybox pattern array.
 * @param pattern_point The local 3D point.
 * @return              The final skybox color.
 */
t_color	uv_skybox_at(t_pattern *pattern, t_tuple pattern_point)
{
	t_uv_coord	uv_coord;

	uv_coord.u = 0.0;
	uv_coord.v = 0.0;
	uv_coord.cube_face = 0;
	cubical_map(pattern_point, &uv_coord);
	return (uv_pattern_at(pattern, uv_coord));
}
