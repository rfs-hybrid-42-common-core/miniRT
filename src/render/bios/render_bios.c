/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_bios.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 14:40:44 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 21:50:37 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "render.h"
#include "mlx.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void fill_bios_background(t_engine *engine)
 * @brief Paints the background of the BIOS utility to a solid blue.
 * @details Operates directly on the MLX memory buffer, painting every pixel in 
 * the window to mimic the classic MS-DOS terminal aesthetic.
 * @param engine The global engine context.
 */
static void	fill_bios_background(t_engine *engine)
{
	int	x;
	int	y;
	int	pos;

	y = -1;
	while (++y < engine->camera.vsize)
	{
		x = -1;
		while (++x < engine->camera.hsize)
		{
			pos = (y * engine->mlx.line_length) + (x * (engine->mlx.bpp / 8));
			*((unsigned int *)(engine->mlx.img_addr + pos)) = 0x0000AA;
		}
	}
	mlx_put_image_to_window(engine->mlx.mlx_ptr, engine->mlx.win_ptr,
		engine->mlx.img_ptr, 0, 0);
}

/**
 * @fn void render_bios(t_engine *engine)
 * @brief State-machine router for the BIOS UI utility.
 * @details Evaluates the current active BIOS page and delegates the text 
 * rendering. Always stamps the persistent BIOS header and instructional 
 * footer strings via the mlx_string_put API.
 * @param engine The global engine context.
 */
void	render_bios(t_engine *engine)
{
	fill_bios_background(engine);
	mlx_string_put(engine->mlx.mlx_ptr, engine->mlx.win_ptr, 50, 50,
		0x00FFFF, "*** miniRT Setup Utility (BIOS) ***");
	if (engine->bios.current_page == PAGE_MAIN_MENU)
		render_main_menu(engine);
	else if (engine->bios.current_page == PAGE_SCENES_WORKING)
		render_file_list(engine, engine->bios.working_scenes);
	else if (engine->bios.current_page == PAGE_SCENES_BROKEN)
		render_file_list(engine, engine->bios.broken_scenes);
	else if (engine->bios.current_page == PAGE_DOCS_PARSER)
		render_docs_parser(engine);
	else if (engine->bios.current_page == PAGE_DOCS_CONTROLS)
		render_docs_controls(engine);
	if (engine->bios.current_page != PAGE_MAIN_MENU)
	{
		mlx_string_put(engine->mlx.mlx_ptr, engine->mlx.win_ptr, 50, 550,
			0xFFFF00, "Press ESC to return to Main Menu");
	}
}
