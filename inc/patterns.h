/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   patterns.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/02 02:43:09 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 16:08:40 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATTERNS_H
# define PATTERNS_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "math_rt.h"
# include "objects.h"

/* ========================================================================== */
/* DATA STRUCTURES                                                            */
/* ========================================================================== */

/**
 * @typedef t_object
 * @brief Forward declaration of the object structure.
 * @details Resolves circular dependencies between objects.h and patterns.h.
 */
typedef struct s_object	t_object;

/**
 * @enum e_pattern_type
 * @brief Enumeration of all supported procedural and texture patterns.
 * @details Used to safely route color evaluation to the appropriate algorithm.
 * @var STRIPE         1D alternating stripes.
 * @var GRADIENT       Linear interpolation between two colors.
 * @var RING           Concentric rings radiating from the origin.
 * @var CHECKER        3D volumetric checkerboard.
 * @var PERLIN         Raw 3D fractal turbulence.
 * @var MARBLE         Sine wave perturbed by fractal turbulence.
 * @var UV_CHECKER     2D surface-wrapped checkerboard.
 * @var UV_IMAGE       2D surface-wrapped image texture (XPM).
 * @var UV_SKYBOX      6-sided environmental texture array.
 * @var PATTTERN_COUNT Total number of pattern types for jump-table sizing.
 */
typedef enum e_pattern_type
{
	STRIPE,
	GRADIENT,
	RING,
	CHECKER,
	PERLIN,
	MARBLE,
	UV_CHECKER,
	UV_IMAGE,
	UV_SKYBOX,
	PATTTERN_COUNT
}	t_pattern_type;

/**
 * @enum e_uv_map
 * @brief Enumeration of UV unwrapping algorithms.
 * @details Dictates how a flat 2D image or checkerboard wraps around 
 * 3D geometry.
 * @var SPHERICAL     Wraps around a sphere using polar coordinates.
 * @var PLANAR        Tiles infinitely across a flat plane.
 * @var CUBICAL       Projects onto a 6-sided bounding box.
 * @var CYLINDRICAL   Wraps the curved wall and concentric caps.
 * @var CONICAL       Wraps a cone shape using slant-height interpolation.
 * @var TOROIDAL      Wraps entirely around a donut shape.
 * @var TOTAL_UV_MAPS Total count of mapping algorithms.
 */
typedef enum e_uv_map
{
	SPHERICAL,
	PLANAR,
	CUBICAL,
	CYLINDRICAL,
	CONICAL,
	TOROIDAL,
	TOTAL_UV_MAPS
}	t_uv_map;

/**
 * @enum e_cube_face
 * @brief Enumeration of the 6 faces of a cube.
 * @details Used specifically for cubical mapping and skybox texture sampling.
 * @var FACE_RIGHT +X axis face.
 * @var FACE_LEFT  -X axis face.
 * @var FACE_UP    +Y axis face.
 * @var FACE_DOWN  -Y axis face.
 * @var FACE_FRONT +Z axis face.
 * @var FACE_BACK  -Z axis face.
 */
typedef enum e_cube_face
{
	FACE_RIGHT,
	FACE_LEFT,
	FACE_UP,
	FACE_DOWN,
	FACE_FRONT,
	FACE_BACK
}	t_cube_face;

/**
 * @struct s_uv_coord
 * @brief Stores calculated 2D mapping coordinates.
 * @details Holds the fractional coordinates mapped from 3D space, heavily used 
 * by the texture fetching functions.
 * @var u         The horizontal mapping coordinate [0.0 to 1.0].
 * @var v         The vertical mapping coordinate [0.0 to 1.0].
 * @var cube_face The active cube face (only relevant for cubical mappings).
 */
typedef struct s_uv_coord
{
	double		u;
	double		v;
	t_cube_face	cube_face;
}	t_uv_coord;

/**
 * @struct s_texture
 * @brief Encapsulates a loaded MiniLibX image buffer.
 * @details Stores the raw memory array and byte-alignment data required for 
 * manual sub-pixel color extraction.
 * @var img_ptr     The MLX image reference pointer.
 * @var pixels      The raw memory string containing the pixel data.
 * @var width       Image width in pixels.
 * @var height      Image height in pixels.
 * @var bpp         Bits per pixel (usually 32 for ARGB/BGRA).
 * @var line_length Bytes per horizontal image line.
 * @var endian      Endianness of the local system (affects byte bitshifting).
 */
typedef struct s_texture
{
	void	*img_ptr;
	char	*pixels;
	int		width;
	int		height;
	int		bpp;
	int		line_length;
	int		endian;
}	t_texture;

/**
 * @struct s_pattern
 * @brief The universal pattern and texture descriptor.
 * @details Holds the specific rendering data required to evaluate procedural 
 * math or texture sample buffers. Uses a polymorphic union for memory 
 * efficiency.
 * @var type                      The active pattern type determining the 
 * evaluation route.
 * @var color_a                   Primary base color for procedural patterns.
 * @var color_b                   Secondary base color for procedural patterns.
 * @var transform                 The spatial transformation matrix for the 
 * pattern.
 * @var transform_inverse         Precomputed inverse matrix for local space 
 * conversion.
 * @var u_data                    Union holding data specific to the active 
 * pattern type.
 * @var u_data.s_uv               Struct holding 2D UV mapping and texture data.
 * @var u_data.s_uv.uv_map        The algorithm used to wrap 2D patterns around 
 * 3D geometry.
 * @var u_data.s_uv.width         The horizontal scale/tile count for 
 * UV checkers.
 * @var u_data.s_uv.height        The vertical scale/tile count for UV checkers.
 * @var u_data.s_uv.texture       Pointer to the loaded MLX image buffer 
 * (for UV_IMAGE).
 * @var u_data.s_skybox           Struct holding environmental skybox data.
 * @var u_data.s_skybox.faces     Array of 6 MLX textures mapping to the faces 
 * of a bounding cube.
 * @var u_data.s_noise            Struct holding procedural noise generation 
 * parameters.
 * @var u_data.s_noise.octaves    Number of fractal noise layers to sum 
 * together.
 * @var u_data.s_noise.vein_scale Density and spatial scaling factor for marble 
 * veins.
 */
typedef struct s_pattern
{
	t_pattern_type	type;
	t_color			color_a;
	t_color			color_b;
	t_matrix		transform;
	t_matrix		transform_inverse;
	union
	{
		struct
		{
			t_uv_map	uv_map;
			int			width;
			int			height;
			t_texture	*texture;
		}	s_uv;
		struct
		{
			t_texture	*faces[6];
		}	s_skybox;
		struct
		{
			int		octaves;
			double	vein_scale;
		}	s_noise;
	}	u_data;
}	t_pattern;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ---------------------------- Config & Routing ---------------------------- */
void		set_pattern_transform(t_pattern *pattern, t_matrix *transform);
t_color		pattern_at_object(t_object *object, t_pattern *pattern,
				t_tuple world_point, double time);

/* ------------------------ Procedural Constructors ------------------------- */
t_pattern	stripe_pattern(t_color color_a, t_color color_b);
t_pattern	gradient_pattern(t_color color_a, t_color color_b);
t_pattern	ring_pattern(t_color color_a, t_color color_b);
t_pattern	checkers_pattern(t_color color_a, t_color color_b);
t_pattern	perlin_pattern(t_color color_a, t_color color_b, int octaves);
t_pattern	marble_pattern(t_color color_a, t_color color_b, int octaves,
				double vein_scale);

/* ------------------------- Procedural Evaluators -------------------------- */
t_color		stripe_at(t_pattern *pattern, t_tuple pattern_point);
t_color		gradient_at(t_pattern *pattern, t_tuple pattern_point);
t_color		ring_at(t_pattern *pattern, t_tuple pattern_point);
t_color		checkers_at(t_pattern *pattern, t_tuple pattern_point);
t_color		perlin_at(t_pattern *pattern, t_tuple pattern_point);
t_color		marble_at(t_pattern *pattern, t_tuple pattern_point);

/* ---------------------------- Noise Generation ---------------------------- */
double		turbulence(t_tuple pattern_point, int octaves);

/* ------------------------------- UV Mapping ------------------------------- */
void		spherical_map(t_tuple point, t_uv_coord *uv_coord);
void		planar_map(t_tuple point, t_uv_coord *uv_coord);
void		cubical_map(t_tuple point, t_uv_coord *uv_coord);
void		cylindrical_map(t_tuple point, t_uv_coord *uv_coord);
void		conical_map(t_tuple point, t_uv_coord *uv_coord);
void		toroidal_map(t_tuple point, t_uv_coord *uv_coord);

/* ---------------------- Image Textures & UV Checkers ---------------------- */
t_pattern	uv_checkers_pattern(int width, int height, t_uv_map map_type,
				t_color *colors);
t_color		uv_checkers_at(t_pattern *pattern, t_tuple point);
t_color		get_pixel_color(t_texture *texture, int x, int y);
t_pattern	uv_image_pattern(void *mlx_ptr, char *filepath, t_uv_map map_type);
t_color		uv_image_at(t_pattern *pattern, t_tuple pattern_point);

/* --------------------------------- Skybox --------------------------------- */
t_pattern	uv_skybox_pattern(void *mlx_ptr, char **filepaths);
t_color		uv_skybox_at(t_pattern *pattern, t_tuple pattern_point);

#endif
