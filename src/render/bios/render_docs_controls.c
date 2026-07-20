/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_docs_controls.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 22:15:01 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/02 21:00:32 by maaugust         ###   ########.fr       */
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
 * @fn void render_docs_controls(t_engine *engine)
 * @brief Renders the textual manual for keyboard and mouse shortcuts.
 * @details Sequentially places strings onto the window. Utilizes standard 
 * Y-axis offset math to ensure consistent line spacing.
 * @param engine The global engine context.
 */
void	render_docs_controls(t_engine *engine)
{
	mlx_string_put(engine->mlx.mlx_ptr, engine->mlx.win_ptr, 100, 150, 0xFFFFFF,
		"W/S: Rotate Up/Down | A/D: Rotate Left/Right | Q/E: Roll CCW/CW");
	mlx_string_put(engine->mlx.mlx_ptr, engine->mlx.win_ptr, 100, 180, 0xFFFFFF,
		"Arrows: Move Camera Up, Down, Left, Right");
	mlx_string_put(engine->mlx.mlx_ptr, engine->mlx.win_ptr, 100, 210, 0xFFFFFF,
		"Mouse Wheel: Zoom in/out at cursor location");
	mlx_string_put(engine->mlx.mlx_ptr, engine->mlx.win_ptr, 100, 240, 0xFFFFFF,
		"1, 2, 3, 4: Antialiasing (None, x4, x9, x16)");
	mlx_string_put(engine->mlx.mlx_ptr, engine->mlx.win_ptr, 100, 270, 0xFFFFFF,
		"P: Pixelated Mode | F: Focal Blur | M: Motion Blur");
}
