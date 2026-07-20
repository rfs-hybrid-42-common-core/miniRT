/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 00:05:59 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/12 22:32:36 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENGINE_H
# define ENGINE_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "camera.h"
# include "mlx.h"
# include "objects.h"
# include "world.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <stdint.h>

/* ========================================================================== */
/* MACROS                                                                     */
/* ========================================================================== */

/**
 * @def FLAG_PIXELATED_MODE
 * @brief Bitmask flag for the pixelated performance mode.
 * @details When active, the renderer scales down the resolution and bypasses 
 * intensive calculations to maintain high framerates during camera movement.
 */
# ifndef FLAG_PIXELATED_MODE
#  define FLAG_PIXELATED_MODE		0x1
# endif

/**
 * @def FLAG_FOCAL_BLUR
 * @brief Bitmask flag for physical depth of field.
 * @details When active, rays are offset across the camera's aperture to 
 * simulate a real-world lens, blurring objects outside the focal plane.
 */
# ifndef FLAG_FOCAL_BLUR
#  define FLAG_FOCAL_BLUR			0x2
# endif

/**
 * @def FLAG_MOTION_BLUR
 * @brief Bitmask flag for temporal motion blur.
 * @details When active, rays sample a random point in time across the 
 * camera's shutter interval, creating a motion streak on moving objects.
 */
# ifndef FLAT_MOTION_BLUR
#  define FLAG_MOTION_BLUR			0x4
# endif

/**
 * @def MAX_FILE_LEN
 * @brief Maximum string length for a parsed scene filename.
 * @details Limits buffer sizes when reading `.rt` files from directories to 
 * populate the BIOS UI lists.
 */
# ifndef MAX_FILE_LEN
#  define MAX_FILE_LEN				256
# endif

/**
 * @def PAGE_MAIN_MENU_ITEMS
 * @brief Total number of selectable items on the BIOS root menu.
 * @details Used to restrict cursor boundary limits.
 */
# ifndef PAGE_MAIN_MENU_ITEMS
#  define PAGE_MAIN_MENU_ITEMS		5
# endif

/**
 * @def PAGE_DOCS_PARSER_ITEMS
 * @brief Total number of scrollable lines in the BIOS manual.
 * @details Used to enforce scrolling clamps.
 */
# ifndef PAGE_DOCS_PARSER_ITEMS
#  define PAGE_DOCS_PARSER_ITEMS	29
# endif

/* ----------------------------- Camera Speeds ------------------------------ */

/**
 * @def CAM_MOVE_SPEED
 * @brief Positional translation delta per keypress.
 * @details Distance the camera travels along its local axes per frame.
 */
# ifndef CAM_MOVE_SPEED
#  define CAM_MOVE_SPEED			0.5
# endif

/**
 * @def CAM_ROTATE_SPEED
 * @brief Rotational translation delta per keypress.
 * @details Angle in radians the camera rotates (Pitch/Yaw/Roll) per frame.
 */
# ifndef CAM_ROTATE_SPEED
#  define CAM_ROTATE_SPEED			0.5
# endif

/**
 * @def CAM_ZOOM_SPEED
 * @brief Positional translation delta per mouse scroll.
 * @details Distance the camera translates forward/backward along a ray cast 
 * from the exact cursor coordinate.
 */
# ifndef CAM_ZOOM_SPEED
#  define CAM_ZOOM_SPEED			1.0
# endif

/* ----------------------------- Mouse Buttons ------------------------------ */

/**
 * @def MOUSE_LEFT
 * @brief X11 constant for Left Mouse Button click.
 * @details Mapped to button 1 in MiniLibX.
 */
# ifndef MOUSE_LEFT
#  define MOUSE_LEFT				1
# endif

/**
 * @def MOUSE_RIGHT
 * @brief X11 constant for Right Mouse Button click.
 * @details Mapped to button 2 in MiniLibX.
 */
# ifndef MOUSE_RIGHT
#  define MOUSE_RIGHT				2
# endif

/**
 * @def MOUSE_MIDDLE
 * @brief X11 constant for Middle Mouse Button (Scrollwheel) click.
 * @details Mapped to button 3 in MiniLibX.
 */
# ifndef MOUSE_MIDDLE
#  define MOUSE_MIDDLE				3
# endif

/**
 * @def MOUSE_SCROLL_UP
 * @brief X11 constant for Mouse Wheel scrolling upwards.
 * @details Mapped to button 4 in MiniLibX.
 */
# ifndef MOUSE_SCROLL_UP
#  define MOUSE_SCROLL_UP			4
# endif

/**
 * @def MOUSE_SCROLL_DOWN
 * @brief X11 constant for Mouse Wheel scrolling downwards.
 * @details Mapped to button 5 in MiniLibX.
 */
# ifndef MOUSE_SCROLL_DOWN
#  define MOUSE_SCROLL_DOWN			5
# endif

/* ========================================================================== */
/* DATA STRUCTURES                                                            */
/* ========================================================================== */

/**
 * @struct s_mlx
 * @brief MiniLibX context manager.
 * @details Encapsulates the display server connection, window interface, and 
 * the memory-mapped image buffer required for high-speed pixel manipulation.
 * @var mlx_ptr     Master pointer to the X-Window system connection.
 * @var win_ptr     Pointer to the specific rendered window instance.
 * @var img_ptr     Pointer to the frame buffer image object.
 * @var img_addr    Direct memory address string to the raw pixel data array.
 * @var bpp         Bits per pixel format (usually 32 for ARGB).
 * @var line_length Row pitch in bytes (used for 2D coordinate calculations).
 * @var endian      Endian format flag of the host OS architecture.
 */
typedef struct s_mlx
{
	void	*mlx_ptr;
	void	*win_ptr;
	void	*img_ptr;
	char	*img_addr;
	int		bpp;
	int		line_length;
	int		endian;
}	t_mlx;

/**
 * @enum e_bios_page
 * @brief State identifiers for the BIOS Utility UI.
 * @details Tracks which specific sub-menu the user is currently navigating.
 * @var PAGE_MAIN_MENU      The root selection hub.
 * @var PAGE_SCENES_WORKING File browser for valid RT files.
 * @var PAGE_SCENES_BROKEN  File browser for intentionally invalid RT files.
 * @var PAGE_DOCS_PARSER    Scrollable text manual for the parsing rules.
 * @var PAGE_DOCS_CONTROLS  Static text manual mapping the keyboard bindings.
 */
typedef enum e_bios_page
{
	PAGE_MAIN_MENU,
	PAGE_SCENES_WORKING,
	PAGE_SCENES_BROKEN,
	PAGE_DOCS_PARSER,
	PAGE_DOCS_CONTROLS
}	t_bios_page;

/**
 * @struct s_bios
 * @brief Controller for the startup BIOS utility.
 * @details Maintains the state of the UI when the engine is launched without 
 * arguments, including cursor positions and sorted directory file lists.
 * @var active         Boolean flag indicating if the BIOS is rendering.
 * @var current_page   The active UI screen identifier.
 * @var cursor_index   The integer position of the user's selection cursor.
 * @var max_items      The scroll/selection limit for the active page.
 * @var working_scenes Pre-loaded linked list of files in `scenes/working/`.
 * @var broken_scenes  Pre-loaded linked list of files in `scenes/broken/`.
 */
typedef struct s_bios
{
	bool		active;
	t_bios_page	current_page;
	int			cursor_index;
	int			max_items;
	t_list		*working_scenes;
	t_list		*broken_scenes;
}	t_bios;

/**
 * @enum e_aa_level
 * @brief Multi-Sampling Anti-Aliasing (MSAA) density parameters.
 * @details Determines the grid size for spatial jittering per pixel.
 * @var AA_NONE Standard 1-ray-per-pixel execution.
 * @var AA_X4   Casts 4 rays per pixel (2x2 grid).
 * @var AA_X9   Casts 9 rays per pixel (3x3 grid).
 * @var AA_X16  Casts 16 rays per pixel (4x4 grid).
 */
typedef enum e_aa_level
{
	AA_NONE	= 1,
	AA_X4	= 2,
	AA_X9	= 3,
	AA_X16	= 4
}	t_aa_level;

/**
 * @enum e_exit_code
 * @brief System termination status identifiers.
 * @details Determines the final return code of the program to the OS shell, 
 * specifically useful for Bash scripting or automated tester tracking.
 * @var SUCCESS   Clean termination (Return 0).
 * @var NUM_ARGS  Invalid command line argument count.
 * @var RT_OPEN   Failure to open the primary .rt file.
 * @var RT_PARSE  Syntax or logic error inside the .rt file.
 * @var XPM_OPEN  Failure to load an external texture file.
 * @var OBJ_OPEN  Failure to open an external .obj mesh file.
 * @var OBJ_PARSE Syntax or logic error inside the .obj file.
 * @var MLX_INIT  Failure of the display server or window manager.
 * @var MALLOC    Catastrophic heap memory allocation failure.
 * @var THREAD    Failure to allocate or launch POSIX render threads.
 */
typedef enum e_exit_code
{
	SUCCESS,
	NUM_ARGS,
	RT_OPEN,
	RT_PARSE,
	XPM_OPEN,
	OBJ_OPEN,
	OBJ_PARSE,
	MLX_INIT,
	MALLOC,
	THREAD
}	t_exit_code;

/**
 * @struct s_engine
 * @brief The Master Context.
 * @details Holds every global subsystem required for the raytracer to run. 
 * This monolithic struct is safely passed by reference to all hooks and loops.
 * @var mlx          The display and graphics context.
 * @var scene_path   The dynamically allocated file path to the active .rt 
 * scene.
 * @var exec_dir     The dynamically allocated prefix path to the executable, 
 * ensuring reliable file loading regardless of the current working directory.
 * @var bios         The UI menu state controller.
 * @var camera       The active viewport matrix and parameters.
 * @var world        The physical scene data (geometry, lights, BVH).
 * @var aa_level     The active anti-aliasing intensity setting.
 * @var aa_jitter    The precomputed spatial noise array for anti-aliasing.
 * @var render_flags Bitmask storing active engine rendering states.
 * @var exit_code    The impending exit status of the program.
 */
typedef struct s_engine
{
	t_mlx		mlx;
	char		*scene_path;
	char		*exec_dir;
	t_bios		bios;
	t_camera	camera;
	t_world		world;
	t_aa_level	aa_level;
	double		*aa_jitter;
	uint8_t		render_flags;
	t_exit_code	exit_code;
}	t_engine;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ----------------------------- Initialization ----------------------------- */
void	init_mlx_window(t_engine *engine);
void	init_engine(char *filepath, t_engine *engine, char *exec_file);
void	init_bios(t_engine *engine);

/* ------------------------------ Event Hooks ------------------------------- */
int		bios_key_hook(int keysym, void *param);
int		scene_key_hook(int keysym, void *param);
int		scene_mouse_hook(int button, int x, int y, void *param);

/* ---------------------- Destruction & Exit Routines ----------------------- */
void	check_rt_extension(char *filepath, t_engine *engine);
void	free_object(t_object *object, void *mlx_ptr);
void	free_memory(t_engine *engine);
void	print_error(char *error_msg, char *filepath);
int		exit_program(t_engine *engine);

#endif
