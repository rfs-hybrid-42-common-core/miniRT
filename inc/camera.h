/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 02:46:43 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/02 20:50:49 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAMERA_H
# define CAMERA_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "math_rt.h"
# include "ray.h"

/* ========================================================================== */
/* DATA STRUCTURES                                                            */
/* ========================================================================== */

/**
 * @typedef t_engine
 * @brief Forward declaration of the global engine context.
 * @details Required to prevent a circular dependency between camera.h and 
 * engine.h. It allows camera functions to accept the engine context as a 
 * pointer without needing its full memory blueprint evaluated beforehand.
 */
typedef struct s_engine	t_engine;

/**
 * @struct s_camera
 * @brief Represents the virtual viewpoint and lens properties of the engine.
 * @details Holds the resolution, field of view, transformation matrices, and 
 * physical lens properties (aperture/focal length) required to cast rays into 
 * the 3D scene and render depth of field.
 * @var hsize             Horizontal resolution of the viewport in pixels.
 * @var vsize             Vertical resolution of the viewport in pixels.
 * @var fov               The Field of View angle (in radians).
 * @var transform         The matrix describing the camera's location and 
 * rotation.
 * @var transform_inverse The inverted transform, used to convert world 
 * coordinates to camera space.
 * @var half_width        Half the width of the camera's viewing canvas.
 * @var half_height       Half the height of the camera's viewing canvas.
 * @var pixel_size        The mathematical size of a single pixel on the canvas.
 * @var aperture          The size of the lens opening (0.0 for a pinhole 
 * camera).
 * @var focal_length      The distance to the focal plane (for depth of field).
 */
typedef struct s_camera
{
	int			hsize;
	int			vsize;
	double		fov;
	t_matrix	transform;
	t_matrix	transform_inverse;
	double		half_width;
	double		half_height;
	double		pixel_size;
	double		aperture;
	double		focal_length;
}	t_camera;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ----------------------------- Initialization ----------------------------- */
t_camera	create_camera(int hsize, int vsize, double fov);
t_matrix	view_transform(t_tuple from, t_tuple to, t_tuple up);

/* ----------------------------- Transformations ---------------------------- */
void		set_camera_transform(t_camera *camera, t_matrix *transform);
void		rotate_camera_transform(t_camera *camera, t_matrix *local_rotation);
void		update_camera_transform(t_camera *camera, t_tuple new_position);

/* ------------------------------ Ray Casting ------------------------------- */
t_ray		ray_for_pixel(t_engine *engine, double pixel_x, double pixel_y,
				double *lens_offset);

#endif
