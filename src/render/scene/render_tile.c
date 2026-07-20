/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_tile.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 23:09:53 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/02 01:41:02 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "render.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void write_pixel(t_engine *engine, int x, int y, int c)
 * @brief Directly manipulates the raw MiniLibX image buffer.
 * @details Bypasses the slow `mlx_pixel_put` API in favor of writing directly 
 * to the underlying continuous memory array. Uses the `line_length` and `bpp` 
 * offsets to jump straight to the correct byte address.
 * @param engine The global engine context.
 * @param x      The X screen coordinate.
 * @param y      The Y screen coordinate.
 * @param c      The processed, integer-based ARGB/BGRA color.
 */
static void	write_pixel(t_engine *engine, int x, int y, int c)
{
	char	*dst;

	if (x < 0 || y < 0 || x >= engine->camera.hsize
		|| y >= engine->camera.vsize)
		return ;
	dst = engine->mlx.img_addr + (y * engine->mlx.line_length
			+ x * (engine->mlx.bpp / 8));
	*(unsigned int *)dst = c;
}

/**
 * @fn static int clamp_color(double c)
 * @brief Maps a floating-point color value into an 8-bit integer limit.
 * @details Multiplies the [0.0 - 1.0] internal floating format by 255. 
 * Safely caps the result between 0 and 255 to prevent overflow visual bugs.
 * @param c The floating point color channel.
 * @return  The safe 8-bit integer value.
 */
static int	clamp_color(double c)
{
	int	val;

	val = (int)(c * 255);
	if (val > 255)
		val = 255;
	if (val < 0)
		val = 0;
	return (val);
}

/**
 * @fn static int color_to_int(t_color c, int endian)
 * @brief Encodes the R, G, and B channels into a single 32-bit integer.
 * @details Employs bitshifting to pack the channels. Crucially, uses the 
 * system's endian flag (provided by MLX) to accurately place the bytes in 
 * either ARGB or BGRA formats depending on whether the OS is Little or 
 * Big Endian.
 * @param c      The internal t_color float struct.
 * @param endian The system-specific endian flag.
 * @return       The packed integer representation.
 */
static int	color_to_int(t_color c, int endian)
{
	if (endian == 1)
		return ((clamp_color(c.b) << 16) | (clamp_color(c.g) << 8)
			| clamp_color(c.r));
	return ((clamp_color(c.r) << 16) | (clamp_color(c.g) << 8)
		| clamp_color(c.b));
}

/**
 * @fn void render_tile_pixelated(t_engine *engine, t_tile *tile, int step)
 * @brief Renders a lower-resolution chunk while overriding buffer ghosting.
 * @details Steps by multiple pixels at a time to drastically reduce ray counts 
 * during real-time movement. Uses a nested loop to fill a solid block of color 
 * over the skipped pixels, ensuring the previous frame's data is crushed to 
 * prevent visual tearing artifacts.
 * @param engine The global engine context.
 * @param tile   The designated boundary struct.
 * @param step   The resolution scale factor (e.g., 4 calculates 1 pixel per 4).
 */
void	render_tile_pixelated(t_engine *engine, t_tile *tile, int step)
{
	t_color	color;
	int		x;
	int		y;
	int		dx;
	int		dy;

	y = tile->start_y;
	while (y < tile->end_y)
	{
		x = tile->start_x;
		while (x < tile->end_x)
		{
			color = sample_pixel(engine, x, y);
			dy = -1;
			while (++dy < step && y + dy < tile->end_y)
			{
				dx = -1;
				while (++dx < step && x + dx < tile->end_x)
					write_pixel(engine, x + dx, y + dy,
						color_to_int(color, engine->mlx.endian));
			}
			x += step;
		}
		y += step;
	}
}

/**
 * @fn void render_tile(t_engine *engine, t_tile *tile)
 * @brief Loops over the bounds of a specific chunk to sample and print pixels.
 * @details Executed by the thread worker. It samples the color for each 
 * coordinate inside the tile boundary and writes it straight to memory.
 * @param engine The global engine context.
 * @param tile   The designated boundary struct.
 */
void	render_tile(t_engine *engine, t_tile *tile)
{
	t_color	c;
	int		x;
	int		y;

	y = tile->start_y - 1;
	while (++y < tile->end_y)
	{
		x = tile->start_x - 1;
		while (++x < tile->end_x)
		{
			c = sample_pixel(engine, x, y);
			write_pixel(engine, x, y, color_to_int(c, engine->mlx.endian));
		}
	}
}
