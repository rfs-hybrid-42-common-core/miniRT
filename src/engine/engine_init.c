/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 00:05:52 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/16 22:38:09 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "engine.h"
#include "math_rt.h"
#include "parser.h"
#include "shading.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stdbool.h>
#include <unistd.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void set_exec_dir(t_engine *engine, char *exec_file)
 * @brief Extracts and caches the executable's directory path.
 * @details Parses the launch command string to isolate the parent directory 
 * of the binary. This cached path acts as an absolute root for all subsequent 
 * file loading operations, ensuring the engine can reliably locate scene files 
 * regardless of the user's current working directory in the terminal.
 * @param engine    Pointer to the global engine context.
 * @param exec_file The raw launch string provided by argv[0].
 */
static void	set_exec_dir(t_engine *engine, char *exec_file)
{
	char	*last_slash;

	last_slash = ft_strrchr(exec_file, '/');
	if (!last_slash)
		engine->exec_dir = ft_strdup("./");
	else
		engine->exec_dir = ft_substr(exec_file, 0, last_slash - exec_file + 1);
	if (!engine->exec_dir)
	{
		engine->exit_code = MALLOC;
		ft_putendl_fd("Error\nMalloc failed for exec_dir", STDERR_FILENO);
		exit_program(engine);
	}
}

/**
 * @fn static void error_detected(t_engine *engine)
 * @brief Triggers a catastrophic display failure abort.
 * @details Invoked if MiniLibX fails to establish a window or image buffer, 
 * typically caused by missing X11 display environments (e.g., running over 
 * SSH).
 * @param engine Pointer to the global engine context.
 */
static void	error_detected(t_engine *engine)
{
	engine->exit_code = MLX_INIT;
	ft_putendl_fd("Error\nMLX setup failed!", STDERR_FILENO);
	exit_program(engine);
}

/**
 * @fn void init_mlx_window(t_engine *engine)
 * @brief Provisions the physical window and backbuffer.
 * @details Opens the OS-level window frame using the resolution parsed from 
 * the scene. Instantiates the massive character array (`img_addr`) where all 
 * raytraced pixels will be directly written.
 * @param engine Pointer to the global engine context.
 */
void	init_mlx_window(t_engine *engine)
{
	engine->mlx.win_ptr = mlx_new_window(engine->mlx.mlx_ptr,
			engine->camera.hsize,
			engine->camera.vsize,
			"miniRT");
	if (!engine->mlx.win_ptr)
		error_detected(engine);
	engine->mlx.img_ptr = mlx_new_image(engine->mlx.mlx_ptr,
			engine->camera.hsize,
			engine->camera.vsize);
	if (!engine->mlx.img_ptr)
		error_detected(engine);
	engine->mlx.img_addr = mlx_get_data_addr(engine->mlx.img_ptr,
			&engine->mlx.bpp,
			&engine->mlx.line_length,
			&engine->mlx.endian);
}

/**
 * @fn void init_engine(char *filepath, t_engine *engine, char *exec_file)
 * @brief Master initialization sequencer for the application.
 * @details Establishes baseline engine states, caches the execution 
 * directory, and initializes the MLX display server connection. If a 
 * file path is provided, it immediately routes to the RT Parser. If no 
 * file is provided, it boots the interactive BIOS Utility.
 * @param filepath  Optional string containing the .rt file to render.
 * @param engine    Pointer to the global engine context.
 * @param exec_file The raw launch command string provided by argv[0].
 */
void	init_engine(char *filepath, t_engine *engine, char *exec_file)
{
	engine->scene_path = NULL;
	engine->exec_dir = NULL;
	set_exec_dir(engine, exec_file);
	engine->aa_level = AA_NONE;
	engine->aa_jitter = generate_jitter_sequence(JITTER_SIZE);
	engine->render_flags = 0;
	engine->exit_code = SUCCESS;
	engine->world = create_world();
	engine->mlx.mlx_ptr = mlx_init();
	if (!engine->mlx.mlx_ptr)
		error_detected(engine);
	engine->camera.hsize = RES_WIDTH_DEFAULT;
	engine->camera.vsize = RES_HEIGHT_DEFAULT;
	if (!filepath)
	{
		engine->bios.active = true;
		init_mlx_window(engine);
		init_bios(engine);
	}
	else
	{
		parse_rt_file(filepath, engine);
		init_mlx_window(engine);
	}
}
