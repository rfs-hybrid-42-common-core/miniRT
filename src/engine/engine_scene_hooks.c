/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine_scene_hooks.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 14:41:07 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/14 03:36:10 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "math_rt.h"
#include "engine.h"
#include "ray.h"
#include "render.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stdlib.h>
#include <X11/keysym.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void perform_antialiasing(int keysym, t_engine *engine)
 * @brief Mutates the global MSAA state based on numeric keypresses.
 * @details Evaluates keys 1-4, sets the appropriate Ray Grid resolution 
 * modifier, and triggers an expensive re-render of the entire frame.
 * @param keysym The X11 identifier for the struck key.
 * @param engine Pointer to the global engine context.
 */
static void	perform_antialiasing(int keysym, t_engine *engine)
{
	if (keysym == XK_1)
		engine->aa_level = AA_NONE;
	else if (keysym == XK_2)
		engine->aa_level = AA_X4;
	else if (keysym == XK_3)
		engine->aa_level = AA_X9;
	else if (keysym == XK_4)
		engine->aa_level = AA_X16;
	else
		return ;
	render_scene(engine);
}

/**
 * @fn static void translate_camera(int keysym, t_engine *engine)
 * @brief Physically moves the camera origin through space.
 * @details Disables Anti-Aliasing to allow for smooth real-time framerates. 
 * Extracts the dynamic Up and Left orientation vectors from the camera's 
 * active transformation matrix, applies the scaling speed modifier, and 
 * alters the positional origin before re-rendering the scene.
 * @param keysym The X11 identifier for the struck key.
 * @param engine Pointer to the global engine context.
 */
static void	translate_camera(int keysym, t_engine *engine)
{
	t_tuple	position;
	t_tuple	left;
	t_tuple	up;

	position = multiply_matrix_tuple(&engine->camera.transform_inverse,
			point(0, 0, 0));
	left = multiply_matrix_tuple(&engine->camera.transform_inverse,
			vector(1, 0, 0));
	up = multiply_matrix_tuple(&engine->camera.transform_inverse,
			vector(0, 1, 0));
	if (keysym == XK_Up)
		position = add_tuples(position, scaling_tuple(up, CAM_MOVE_SPEED));
	else if (keysym == XK_Down)
		position = subtract_tuples(position, scaling_tuple(up, CAM_MOVE_SPEED));
	else if (keysym == XK_Left)
		position = add_tuples(position, scaling_tuple(left, CAM_MOVE_SPEED));
	else if (keysym == XK_Right)
		position = subtract_tuples(position,
				scaling_tuple(left, CAM_MOVE_SPEED));
	else
		return ;
	update_camera_transform(&engine->camera, position);
	render_scene(engine);
}

/**
 * @fn static void rotate_camera(int keysym, t_engine *engine)
 * @brief Applies angular modification to the camera's orientation matrix.
 * @details Applies standard Pitch (W/S), Yaw (A/D), and Roll (Q/E) rotation 
 * matrices to effectively spin the camera's lens inside the 3D space.
 * @param keysym The X11 identifier for the struck key.
 * @param engine Pointer to the global engine context.
 */
static void	rotate_camera(int keysym, t_engine *engine)
{
	t_matrix	rotation;

	if (keysym == XK_w)
		rotation = rotation_matrix_x(CAM_ROTATE_SPEED);
	else if (keysym == XK_s)
		rotation = rotation_matrix_x(-CAM_ROTATE_SPEED);
	else if (keysym == XK_a)
		rotation = rotation_matrix_y(-CAM_ROTATE_SPEED);
	else if (keysym == XK_d)
		rotation = rotation_matrix_y(CAM_ROTATE_SPEED);
	else if (keysym == XK_q)
		rotation = rotation_matrix_z(-CAM_ROTATE_SPEED);
	else if (keysym == XK_e)
		rotation = rotation_matrix_z(CAM_ROTATE_SPEED);
	else
		return ;
	rotate_camera_transform(&engine->camera, &rotation);
	render_scene(engine);
}

/**
 * @fn int scene_key_hook(int keysym, void *param)
 * @brief X11 Event Listener active exclusively during active rendering.
 * @details Intercepts and delegates keyboard events. Controls instant escape, 
 * render flag toggles (pixelated mode, focal blur, motion blur), antialiasing 
 * swaps, and spatial/angular translation logic.
 * @param keysym The X11 identifier for the struck key.
 * @param param  Void pointer to the engine context.
 * @return       EXIT_SUCCESS for the MLX loop handler.
 */
int	scene_key_hook(int keysym, void *param)
{
	t_engine	*engine;

	engine = (t_engine *)param;
	if (keysym == XK_Escape)
		exit_program(engine);
	if (keysym == XK_p || keysym == XK_f || keysym == XK_m)
	{
		if (keysym == XK_p)
			engine->render_flags ^= FLAG_PIXELATED_MODE;
		else if (keysym == XK_f)
			engine->render_flags ^= FLAG_FOCAL_BLUR;
		else
			engine->render_flags ^= FLAG_MOTION_BLUR;
		render_scene(engine);
	}
	if ((keysym >= XK_1 && keysym <= XK_4))
		perform_antialiasing(keysym, engine);
	if (keysym >= XK_Left && keysym <= XK_Down)
		translate_camera(keysym, engine);
	if (keysym == XK_w || keysym == XK_s || keysym == XK_a || keysym == XK_d
		|| keysym == XK_q || keysym == XK_e)
		rotate_camera(keysym, engine);
	return (EXIT_SUCCESS);
}

/**
 * @fn int scene_mouse_hook(int button, int x, int y, void *param)
 * @brief Intercepts physical mouse interaction.
 * @details Calculates a mathematical ray originating at the exact 2D pixel 
 * coordinate currently touched by the mouse. Propels the camera's position 
 * positively or negatively along that specific ray to achieve point-and-zoom 
 * functionality. Disables AA for smoothness.
 * @param button The X11 identifier for the active mouse action.
 * @param x      X screen coordinate of the cursor.
 * @param y      Y screen coordinate of the cursor.
 * @param param  Void pointer to the engine context.
 * @return       EXIT_SUCCESS for the MLX loop handler.
 */
int	scene_mouse_hook(int button, int x, int y, void *param)
{
	t_engine	*engine;
	t_tuple		position;
	t_ray		mouse_ray;
	double		lens_offset[2];

	engine = (t_engine *)param;
	if (button != MOUSE_SCROLL_UP && button != MOUSE_SCROLL_DOWN)
		return (EXIT_SUCCESS);
	lens_offset[0] = 0.0;
	lens_offset[1] = 0.0;
	position = multiply_matrix_tuple(&engine->camera.transform_inverse,
			point(0, 0, 0));
	mouse_ray = ray_for_pixel(engine, x, y, lens_offset);
	if (button == MOUSE_SCROLL_UP)
		position = add_tuples(position, scaling_tuple(mouse_ray.direction,
					CAM_ZOOM_SPEED));
	else if (button == MOUSE_SCROLL_DOWN)
		position = subtract_tuples(position, scaling_tuple(mouse_ray.direction,
					CAM_ZOOM_SPEED));
	update_camera_transform(&engine->camera, position);
	render_scene(engine);
	return (EXIT_SUCCESS);
}
