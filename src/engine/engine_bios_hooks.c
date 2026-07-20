/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine_bios_hooks.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 14:29:54 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/13 23:38:11 by maaugust         ###   ########.fr       */
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
#include <X11/keysym.h>
#include <X11/X.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void handle_main_menu_selection(t_engine *engine)
 * @brief Evaluates an 'Enter' keypress on the Main Menu.
 * @details Triggers state transitions, pushing the engine into the targeted 
 * sub-menus while accurately updating the active cursor boundary limits to 
 * match the size of the target lists/text arrays.
 * @param engine Pointer to the global engine context.
 */
static void	handle_main_menu_selection(t_engine *engine)
{
	int	index;

	index = engine->bios.cursor_index;
	if (index == 0)
	{
		engine->bios.current_page = PAGE_SCENES_WORKING;
		engine->bios.max_items = ft_lstsize(engine->bios.working_scenes);
	}
	else if (index == 1)
	{
		engine->bios.current_page = PAGE_SCENES_BROKEN;
		engine->bios.max_items = ft_lstsize(engine->bios.broken_scenes);
	}
	else if (index == 2)
	{
		engine->bios.current_page = PAGE_DOCS_PARSER;
		engine->bios.max_items = PAGE_DOCS_PARSER_ITEMS;
	}
	else if (index == 3)
		engine->bios.current_page = PAGE_DOCS_CONTROLS;
	else if (index == 4)
		exit_program(engine);
	if (engine->bios.current_page != PAGE_MAIN_MENU)
		engine->bios.cursor_index = 0;
}

/**
 * @fn void check_rt_extension(char *filepath, t_engine *engine)
 * @brief Validates argument format prior to parser initialization.
 * @details Safely evaluates the provided file path string to ensure it exists 
 * and meets the strict `.rt` extension requirement mandated by the project 
 * subject before allowing the engine to attempt opening the file.
 * @param filepath The argument string representing the file path.
 * @param engine   Pointer to the global engine context.
 */
void	check_rt_extension(char *filepath, t_engine *engine)
{
	size_t	len;

	if (!filepath)
	{
		engine->exit_code = MALLOC;
		print_error("Malloc failed for scene path", NULL);
		exit_program(engine);
	}
	len = ft_strlen(filepath);
	if (len < 3 || ft_strncmp(filepath + len - 3, ".rt", 3) != 0)
	{
		engine->exit_code = RT_OPEN;
		ft_putendl_fd("Error\nFile must have a .rt extension", STDERR_FILENO);
		exit_program(engine);
	}
}

/**
 * @fn static void launch_scene(t_engine *engine, char *dir, t_list *scene_list)
 * @brief Transitions the engine from BIOS mode into Raytracing mode.
 * @details Dynamically strings together the target filename with the root 
 * directory path and dismantles the existing BIOS window frame. It validates 
 * the constructed path before passing execution to the RT Parser. Upon 
 * successful parsing, it instantiates a fresh window, triggers the Raytracer, 
 * and binds the Scene event hooks.
 * @param engine     Pointer to the global engine context.
 * @param dir        The directory prefix to prepend to the filename.
 * @param scene_list The list node containing the targeted filename.
 */
static void	launch_scene(t_engine *engine, char *dir, t_list *scene_list)
{
	int		i;

	i = 0;
	while (scene_list && i++ < engine->bios.cursor_index)
		scene_list = scene_list->next;
	if (!scene_list)
		return (free(dir));
	engine->scene_path = ft_strjoin(dir, (char *)scene_list->content);
	free(dir);
	mlx_destroy_image(engine->mlx.mlx_ptr, engine->mlx.img_ptr);
	engine->mlx.img_ptr = NULL;
	mlx_destroy_window(engine->mlx.mlx_ptr, engine->mlx.win_ptr);
	engine->mlx.win_ptr = NULL;
	check_rt_extension(engine->scene_path, engine);
	parse_rt_file(engine->scene_path, engine);
	free(engine->scene_path);
	engine->scene_path = NULL;
	init_mlx_window(engine);
	engine->bios.active = false;
	render_scene(engine);
	mlx_key_hook(engine->mlx.win_ptr, scene_key_hook, engine);
	mlx_mouse_hook(engine->mlx.win_ptr, scene_mouse_hook, engine);
	mlx_hook(engine->mlx.win_ptr, DestroyNotify, NoEventMask, exit_program,
		engine);
}

/**
 * @fn static void handle_enter(t_engine *engine)
 * @brief Routes an 'Enter' keypress based on the active BIOS state.
 * @details Intercepts the submit action and delegates it to the Main Menu 
 * router, or constructs the absolute target path and triggers a scene launch 
 * if the user is currently navigating one of the file browser lists.
 * @param engine Pointer to the global engine context.
 */
static void	handle_enter(t_engine *engine)
{
	char	*path;
	t_list	*target_list;

	if (engine->bios.current_page == PAGE_MAIN_MENU)
		return (handle_main_menu_selection(engine));
	if (engine->bios.current_page == PAGE_SCENES_WORKING)
	{
		target_list = engine->bios.working_scenes;
		path = ft_strjoin(engine->exec_dir, "scenes/working/");
	}
	else if (engine->bios.current_page == PAGE_SCENES_BROKEN)
	{
		target_list = engine->bios.broken_scenes;
		path = ft_strjoin(engine->exec_dir, "scenes/broken/");
	}
	else
		return ;
	if (!path)
	{
		engine->exit_code = MALLOC;
		print_error("Malloc failed for BIOS scene path", NULL);
		exit_program(engine);
	}
	launch_scene(engine, path, target_list);
}

/**
 * @fn int bios_key_hook(int keysym, void *param)
 * @brief X11 Event Listener active exclusively during the BIOS.
 * @details Intercepts standard navigational keys (Up, Down, Enter, Escape). 
 * Escaping pushes the state back up the tree to the Main Menu. Arrow keys 
 * modulate the active cursor within bounded limits.
 * @param keysym The X11 identifier for the struck key.
 * @param param  Void pointer to the engine context.
 * @return       EXIT_SUCCESS for the MLX loop handler.
 */
int	bios_key_hook(int keysym, void *param)
{
	t_engine	*engine;

	engine = (t_engine *)param;
	if (keysym == XK_Escape)
	{
		if (engine->bios.current_page == PAGE_MAIN_MENU)
			exit_program(engine);
		engine->bios.current_page = PAGE_MAIN_MENU;
		engine->bios.cursor_index = 0;
		engine->bios.max_items = PAGE_MAIN_MENU_ITEMS;
	}
	else if (keysym == XK_Up && engine->bios.cursor_index > 0)
		engine->bios.cursor_index--;
	else if (keysym == XK_Down
		&& engine->bios.cursor_index < engine->bios.max_items - 1)
		engine->bios.cursor_index++;
	else if (keysym == XK_Return || keysym == XK_KP_Enter)
		handle_enter(engine);
	if (engine->bios.active)
		render_bios(engine);
	return (EXIT_SUCCESS);
}
