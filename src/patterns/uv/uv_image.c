/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uv_image.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 01:38:59 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:43:47 by maaugust         ###   ########.fr       */
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
 * @fn t_color get_pixel_color(t_texture *texture, int x, int y)
 * @brief Retrieves a specific pixel from the MiniLibX image buffer.
 * @details Safely bounds the x/y checks to prevent segmentation faults. 
 * Dynamically checks the system's endianness to ensure the byte bitshifts 
 * accurately decode the ARGB/BGRA memory payload regardless of the OS 
 * (Linux/macOS).
 * @param texture Pointer to the loaded texture.
 * @param x       The X pixel coordinate.
 * @param y       The Y pixel coordinate.
 * @return        The decoded, floating-point t_color struct.
 */
t_color	get_pixel_color(t_texture *texture, int x, int y)
{
	char			*dst;
	unsigned int	raw_color;
	double			rgb[3];

	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x >= texture->width)
		x = texture->width - 1;
	if (y >= texture->height)
		y = texture->height - 1;
	dst = texture->pixels + (y * texture->line_length + x * (texture->bpp / 8));
	raw_color = *(unsigned int *)dst;
	if (texture->endian == 1)
	{
		rgb[2] = (double)((raw_color >> 16) & 0xFF) / 255.0;
		rgb[1] = (double)((raw_color >> 8) & 0xFF) / 255.0;
		rgb[0] = (double)(raw_color & 0xFF) / 255.0;
		return (color(rgb[0], rgb[1], rgb[2]));
	}
	rgb[0] = (double)((raw_color >> 16) & 0xFF) / 255.0;
	rgb[1] = (double)((raw_color >> 8) & 0xFF) / 255.0;
	rgb[2] = (double)(raw_color & 0xFF) / 255.0;
	return (color(rgb[0], rgb[1], rgb[2]));
}

/**
 * @fn t_pattern uv_image_pattern(void *mlx_ptr, char *filepath, 
 * t_uv_map map_type)
 * @brief Initializes a 2D image texture mapped to a 3D surface.
 * @details Uses MiniLibX to convert an XPM file into a memory buffer. If the 
 * file is missing or corrupted, this implements a "Missing Texture" safeguard 
 * that instantly downgrades the material to a high-contrast Magenta/Black 
 * UV checkerboard to prevent the engine from crashing.
 * @param mlx_ptr  The active MiniLibX instance.
 * @param filepath The path to the .xpm file.
 * @param map_type The algorithm to use for wrapping (Spherical, Planar, etc.).
 * @return         The configured pattern struct.
 */
t_pattern	uv_image_pattern(void *mlx_ptr, char *filepath, t_uv_map map_type)
{
	t_pattern	pattern;
	t_texture	*texture;

	pattern.type = UV_IMAGE;
	pattern.transform = identity_matrix();
	pattern.transform_inverse = pattern.transform;
	pattern.u_data.s_uv.uv_map = map_type;
	texture = malloc(sizeof(t_texture));
	pattern.u_data.s_uv.texture = texture;
	if (texture)
		texture->img_ptr = mlx_xpm_file_to_image(mlx_ptr, filepath,
				&texture->width, &texture->height);
	if (!texture || !texture->img_ptr)
	{
		printf("Warning: Could not load texture '%s'\n", filepath);
		pattern.type = UV_CHECKER;
		pattern.color_a = color(1, 0, 1);
		pattern.color_b = color(0, 0, 0);
		pattern.u_data.s_uv.width = 16;
		pattern.u_data.s_uv.height = 16;
		return (pattern.u_data.s_uv.texture = NULL, free(texture), pattern);
	}
	texture->pixels = mlx_get_data_addr(texture->img_ptr, &texture->bpp,
			&texture->line_length, &texture->endian);
	return (pattern);
}

/**
 * @fn static t_color uv_pattern_at(t_pattern *pattern, t_uv_coord uv_coord)
 * @brief Extracts the exact color from the texture buffer.
 * @details Implements true Bilinear Interpolation. By tracking the exact 
 * floating-point UV location, it samples the 4 surrounding pixels and blends 
 * them according to proximity. This permanently eliminates pixelation 
 * (Minecraft blocks) when the camera gets extremely close to the texture.
 * @param pattern  The image pattern.
 * @param uv_coord The calculated 2D U/V mapping.
 * @return         The bilinearly smoothed color.
 */
static t_color	uv_pattern_at(t_pattern *pattern, t_uv_coord uv_coord)
{
	t_texture	*texture;
	double		uv_exact[2];
	int			x[2];
	int			y[2];
	t_color		top_bottom[2];

	texture = pattern->u_data.s_uv.texture;
	uv_exact[0] = uv_coord.u * (texture->width - 1);
	uv_exact[1] = (1.0 - uv_coord.v) * (texture->height - 1);
	x[0] = floor(uv_exact[0]);
	x[1] = x[0] + 1.0;
	y[0] = floor(uv_exact[1]);
	y[1] = y[0] + 1.0;
	top_bottom[0] = interpolation_color(get_pixel_color(texture, x[0], y[0]),
			get_pixel_color(texture, x[1], y[0]), uv_exact[0] - x[0]);
	top_bottom[1] = interpolation_color(get_pixel_color(texture, x[0], y[1]),
			get_pixel_color(texture, x[1], y[1]), uv_exact[0] - x[0]);
	return (interpolation_color(top_bottom[0], top_bottom[1],
			uv_exact[1] - y[0]));
}

/**
 * @fn t_color uv_image_at(t_pattern *pattern, t_tuple pattern_point)
 * @brief Entry point for evaluating an image texture.
 * @details Executes the specific wrapping algorithm tied to the pattern before 
 * fetching the pixel from the buffer.
 * @param pattern       The image pattern.
 * @param pattern_point The local 3D coordinate.
 * @return              The final texture color.
 */
t_color	uv_image_at(t_pattern *pattern, t_tuple pattern_point)
{
	t_uv_coord	uv_coord;

	uv_coord.u = 0.0;
	uv_coord.v = 0.0;
	uv_coord.cube_face = 0;
	if (pattern->u_data.s_uv.uv_map == SPHERICAL)
		spherical_map(pattern_point, &uv_coord);
	if (pattern->u_data.s_uv.uv_map == PLANAR)
		planar_map(pattern_point, &uv_coord);
	if (pattern->u_data.s_uv.uv_map == CUBICAL)
		cubical_map(pattern_point, &uv_coord);
	if (pattern->u_data.s_uv.uv_map == CYLINDRICAL)
		cylindrical_map(pattern_point, &uv_coord);
	if (pattern->u_data.s_uv.uv_map == CONICAL)
		conical_map(pattern_point, &uv_coord);
	if (pattern->u_data.s_uv.uv_map == TOROIDAL)
		toroidal_map(pattern_point, &uv_coord);
	return (uv_pattern_at(pattern, uv_coord));
}
