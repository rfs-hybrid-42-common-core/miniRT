/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   miniRT.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 02:36:42 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/12 21:36:53 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "engine.h"
#include "libft.h"
#include "mlx.h"
#include "parser.h"
#include "render.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stdlib.h>
#include <unistd.h>
#include <X11/X.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void setup_engine(int argc, char **argv, t_engine *engine)
 * @brief Evaluates command-line arguments to establish the boot sequence.
 * @details If 0 arguments are passed, it triggers the BIOS Utility boot route. 
 * If 1 argument is passed, it validates it and triggers the immediate parse 
 * route. If more than 1 argument is passed, it aborts per the subject rules.
 * @param argc   The terminal argument count.
 * @param argv   Array of argument strings.
 * @param engine Pointer to the global engine context.
 */
static void	setup_engine(int argc, char **argv, t_engine *engine)
{
	if (argc < 2)
		init_engine(NULL, engine, argv[0]);
	else if (argc == 2)
	{
		check_rt_extension(argv[1], engine);
		init_engine(argv[1], engine, argv[0]);
	}
	else
	{
		ft_putendl_fd("Error\nToo many arguments detected!", STDERR_FILENO);
		exit(NUM_ARGS);
	}
}

/**
 * @fn static void start_engine(t_engine *engine)
 * @brief Initiates rendering and passes control to the MLX loop.
 * @details Checks the boot state. If the BIOS is flagged active, it binds the 
 * specific BIOS event handlers. Otherwise, it triggers the Raytracer and binds 
 * the Scene event handlers. Commits the DestroyNotify (Red X) listener before 
 * permanently suspending the main thread to hand infinite control to MLX.
 * @param engine Pointer to the global engine context.
 */
static void	start_engine(t_engine *engine)
{
	if (engine->bios.active)
	{
		render_bios(engine);
		mlx_hook(engine->mlx.win_ptr, KeyPress, KeyPressMask,
			bios_key_hook, engine);
	}
	else
	{
		render_scene(engine);
		mlx_key_hook(engine->mlx.win_ptr, scene_key_hook, engine);
		mlx_mouse_hook(engine->mlx.win_ptr, scene_mouse_hook, engine);
	}
	mlx_hook(engine->mlx.win_ptr, DestroyNotify, NoEventMask, exit_program,
		engine);
	mlx_loop(engine->mlx.mlx_ptr);
}

/**
 * @fn int main(int argc, char **argv)
 * @brief The Master execution point.
 * @details Instantiates the Engine State directly on the stack to ensure an 
 * absolute starting baseline of 0 memory leaks. Bzeros the entire memory 
 * block, passes it into setup, executes the loop, and safely returns 0 upon 
 * completion.
 * @param argc The terminal argument count.
 * @param argv Array of argument strings.
 * @return     EXIT_SUCCESS when the MLX loop is safely broken.
 */
int	main(int argc, char **argv)
{
	t_engine	engine;

	ft_bzero(&engine, sizeof(t_engine));
	setup_engine(argc, argv, &engine);
	start_engine(&engine);
	return (EXIT_SUCCESS);
}
