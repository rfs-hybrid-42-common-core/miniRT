/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 03:06:47 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/02 20:36:47 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "camera.h"
#include "engine.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn t_camera create_camera(int hsize, int vsize, double fov)
 * @brief Initializes a new camera instance with default optical properties.
 * @details Calculates the physical size of the canvas and individual pixels 
 * based on the aspect ratio and field of view. Sets default aperture to 0.0 
 * (pinhole) and focal length to 1.0.
 * @param hsize Horizontal resolution in pixels.
 * @param vsize Vertical resolution in pixels.
 * @param fov   Field of view in radians.
 * @return      A fully initialized t_camera structure.
 */
t_camera	create_camera(int hsize, int vsize, double fov)
{
	t_matrix	identity;
	double		halfview;
	double		aspect;

	identity = identity_matrix();
	halfview = tan(fov / 2.0);
	aspect = (double)hsize / vsize;
	if (aspect >= 1.0)
		return ((t_camera){hsize, vsize, fov, identity, identity, halfview,
			halfview / aspect, (halfview * 2.0) / hsize, 0.0, 1.0});
	return ((t_camera){hsize, vsize, fov, identity, identity, halfview * aspect,
		halfview, (halfview * aspect * 2.0) / hsize, 0.0, 1.0});
}

/**
 * @fn void set_camera_transform(t_camera *camera, t_matrix *transform)
 * @brief Applies a new transformation matrix to the camera.
 * @details Updates the camera's transform and immediately caches its inverse, 
 * which is heavily used during the ray casting phase.
 * @param camera    Pointer to the camera to update.
 * @param transform Pointer to the new transformation matrix.
 */
void	set_camera_transform(t_camera *camera, t_matrix *transform)
{
	camera->transform = *transform;
	camera->transform_inverse = inverse_matrix(&camera->transform);
}

/**
 * @fn void rotate_camera_transform(t_camera *camera, t_matrix *local_rotation)
 * @brief Rotates the camera relative to its current orientation.
 * @details Extracts the camera's local forward and up vectors before applying 
 * the rotation to prevent gimbal lock and ensure 6-DOF movement.
 * @param camera         Pointer to the camera to rotate.
 * @param local_rotation Matrix representing the rotational change.
 */
void	rotate_camera_transform(t_camera *camera, t_matrix *local_rotation)
{
	t_matrix	new_inverse;
	t_tuple		forward;
	t_tuple		up;
	t_tuple		position;
	t_matrix	new_transform;

	new_inverse = multiply_matrices(&camera->transform_inverse, local_rotation);
	position = multiply_matrix_tuple(&new_inverse, point(0, 0, 0));
	forward = multiply_matrix_tuple(&new_inverse, vector(0, 0, -1));
	up = multiply_matrix_tuple(&new_inverse, vector(0, 1, 0));
	new_transform = view_transform(position, add_tuples(position, forward), up);
	set_camera_transform(camera, &new_transform);
}

/**
 * @fn void update_camera_transform(t_camera *camera, t_tuple new_position)
 * @brief Translates the camera to a new physical location.
 * @details Retains the camera's existing forward and up vectors, shifting 
 * only its point of origin.
 * @param camera       Pointer to the camera to move.
 * @param new_position The new absolute coordinates in world space.
 */
void	update_camera_transform(t_camera *camera, t_tuple new_position)
{
	t_tuple		forward;
	t_tuple		up;
	t_matrix	new_transform;

	forward = multiply_matrix_tuple(&camera->transform_inverse,
			vector(0, 0, -1));
	up = multiply_matrix_tuple(&camera->transform_inverse, vector(0, 1, 0));
	new_transform = view_transform(new_position,
			add_tuples(new_position, forward), up);
	set_camera_transform(camera, &new_transform);
}

/**
 * @fn t_ray ray_for_pixel(t_engine *engine, double pixel_x, double pixel_y, 
 * double *lens_offset)
 * @brief Generates a ray originating from the camera through a specific pixel.
 * @details Computes the world-space target based on the focal length. If the 
 * focal blur flag is active, it offsets the ray's origin based on the aperture 
 * size and provided lens offset, simulating physical Depth of Field (Bokeh).
 * @param engine      Pointer to the global engine context.
 * @param pixel_x     The x-coordinate of the pixel on the canvas.
 * @param pixel_y     The y-coordinate of the pixel on the canvas.
 * @param lens_offset A 2D array containing the randomized [u, v] aperture 
 * jitter.
 * @return            A normalized t_ray fired into the scene.
 */
t_ray	ray_for_pixel(t_engine *engine, double pixel_x, double pixel_y,
	double *lens_offset)
{
	t_tuple		origin;
	t_tuple		direction;
	t_tuple		focal_point;
	double		world_x;
	double		world_y;

	world_x = engine->camera.half_width
		- (pixel_x * engine->camera.pixel_size);
	world_y = engine->camera.half_height
		- (pixel_y * engine->camera.pixel_size);
	focal_point = multiply_matrix_tuple(&engine->camera.transform_inverse,
			point(world_x * engine->camera.focal_length,
				world_y * engine->camera.focal_length,
				-engine->camera.focal_length));
	if (engine->render_flags & FLAG_FOCAL_BLUR)
		origin = multiply_matrix_tuple(&engine->camera.transform_inverse,
				point(lens_offset[0] * engine->camera.aperture,
					lens_offset[1] * engine->camera.aperture,
					0));
	else
		origin = multiply_matrix_tuple(&engine->camera.transform_inverse,
				point(0, 0, 0));
	direction = normalize(subtract_tuples(focal_point, origin));
	return (create_ray(origin, direction, 0.0));
}
