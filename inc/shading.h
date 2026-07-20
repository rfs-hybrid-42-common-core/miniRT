/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shading.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 20:09:49 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/03 22:15:22 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADING_H
# define SHADING_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "math_rt.h"
# include "objects.h"
# include "ray.h"
# include "world.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <stdbool.h>

/* ========================================================================== */
/* MACROS                                                                     */
/* ========================================================================== */

/**
 * @def SURFACE_BIAS
 * @brief Offset applied to hit points to prevent shadow acne.
 * @details Pushes the origin of reflection/refraction/shadow rays slightly 
 * outward along the surface normal to escape floating-point rounding errors.
 */
# ifndef SURFACE_BIAS
#  define SURFACE_BIAS		2.5e-3
# endif

/**
 * @def MAX_CONTAINERS
 * @brief Maximum depth of nested refractive objects.
 * @details Limits the array size used to track refractive indices when a ray 
 * travels through overlapping transparent objects (like a glass sphere inside 
 * a tank of water).
 */
# ifndef MAX_CONTAINERS
#  define MAX_CONTAINERS	64
# endif

/**
 * @def JITTER_SIZE
 * @brief Size of the precomputed noise array for area lights.
 * @details By precomputing 4096 random offsets at initialization instead of 
 * calling rand() during the render loop, soft shadow performance is 
 * dramatically increased while maintaining thread safety.
 */
# ifndef JITTER_SIZE
#  define JITTER_SIZE		4096
# endif

/**
 * @def SPARSE_THRESHOLD
 * @brief Defines the sample count threshold for triggering area light 
 * optimization.
 * @details Determines when the engine switches from brute-force penumbra 
 * calculation to the adaptive 16-point sparse grid check. Area lights with a 
 * sample count strictly greater than this value will use the optimization to 
 * safely save CPU cycles, while lights with this number of samples or fewer 
 * will be brute-forced to avoid redundant calculation overhead.
 */
# ifndef SPARSE_THRESHOLD
#  define SPARSE_THRESHOLD	16
# endif

/* ========================================================================== */
/* DATA STRUCTURES                                                            */
/* ========================================================================== */

/**
 * @enum e_light_type
 * @brief Enumeration of supported light sources.
 * @details Used to properly route intensity and attenuation math.
 * @var LIGHT_POINT Radiates light equally in all directions from a single 
 * point.
 * @var LIGHT_AREA  A 2D physical surface emitting light, generating soft 
 * shadows.
 * @var LIGHT_SPOT  A directional cone of light with an inner and outer falloff.
 */
typedef enum e_light_type
{
	LIGHT_POINT,
	LIGHT_AREA,
	LIGHT_SPOT
}	t_light_type;

/**
 * @struct s_light
 * @brief Universal container for light sources.
 * @details Contains all necessary parameters for point, spot, and area lights. 
 * Unused fields for a specific light type simply remain at 0.
 * @var type         The specific type of light source.
 * @var position     The physical origin of the light in 3D space.
 * @var direction    The normalized target vector (Spotlights only).
 * @var inner_cutoff The angle where light begins to fade (Spotlights only).
 * @var outer_cutoff The angle where light completely dies (Spotlights only).
 * @var intensity    The RGB color and brightness of the light.
 * @var uvec         The horizontal width vector of an area light.
 * @var usteps       The number of horizontal sampling cells in an area light.
 * @var vvec         The vertical height vector of an area light.
 * @var vsteps       The number of vertical sampling cells in an area light.
 * @var samples      Total sampling cells (usteps * vsteps).
 * @var center       The absolute spatial center of an area light.
 * @var jitter       Precomputed array of random offsets for soft shadow noise.
 * @var jitter_size  The total size of the allocated jitter array.
 */
typedef struct s_light
{
	t_light_type	type;
	t_tuple			position;
	t_tuple			direction;
	double			inner_cutoff;
	double			outer_cutoff;
	t_color			intensity;
	t_tuple			uvec;
	int				usteps;
	t_tuple			vvec;
	int				vsteps;
	int				samples;
	t_tuple			center;
	double			*jitter;
	int				jitter_size;
}	t_light;

/**
 * @struct s_computations
 * @brief Precomputed geometric data for a specific intersection point.
 * @details Calculated once per hit to prevent redundant math operations during 
 * the multi-pass shading pipeline (ambient, diffuse, specular, reflection).
 * @var t           The exact distance from the ray origin to the intersection.
 * @var object      Pointer to the object that was struck.
 * @var point       The absolute 3D world coordinate of the hit.
 * @var over_point  A point slightly above the surface (prevents shadow acne).
 * @var under_point A point slightly below the surface (for refraction rays).
 * @var eyev        The vector pointing from the hit back to the camera/eye.
 * @var normalv     The surface normal vector (optionally perturbed by bump 
 * maps).
 * @var reflectv    The geometric reflection vector bouncing off the normal.
 * @var inside      True if the ray struck the object from the inside out.
 * @var n1          The refractive index of the material being exited.
 * @var n2          The refractive index of the material being entered.
 * @var time        The temporal parameter for the ray (motion blur).
 */
typedef struct s_computations
{
	double		t;
	t_object	*object;
	t_tuple		point;
	t_tuple		over_point;
	t_tuple		under_point;
	t_tuple		eyev;
	t_tuple		normalv;
	t_tuple		reflectv;
	bool		inside;
	double		n1;
	double		n2;
	double		time;
}	t_computations;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ----------------------- Computations & State Prep ------------------------ */
t_computations	prepare_computations(t_intersection *hit, t_ray *ray,
					t_intersections *xs);

/* ------------------------- Lighting Constructors -------------------------- */
t_light			*create_point_light(t_tuple position, t_color intensity);
t_light			*create_area_light(t_tuple corner, t_tuple *full_uv,
					int *uvsteps, t_color intensity);

/* -------------------------- Area Light Sampling --------------------------- */
t_tuple			sample_area_light(t_light *light, t_tuple point, int u, int v);
double			*generate_jitter_sequence(int count);
double			get_jitter(t_light *light, t_tuple point, int u, int v);

/* -------------------------------- Shading --------------------------------- */
double			intensity_at(t_world *world, t_tuple over_point, t_light *light,
					double time);
t_color			ambient_lighting(t_world *world, t_computations *comps);
t_color			lighting(t_light *light, t_computations *comps,
					double intensity);
t_color			shade_hit(t_world *world, t_computations *comps, int remaining);
t_color			color_at(t_world *world, t_ray *ray, int remaining);

/* ---------------------------- Optics & Physics ---------------------------- */
t_color			reflected_color(t_world *world, t_computations *comps,
					int remaining);
t_color			refracted_color(t_world *world, t_computations *comps,
					int remaining);
double			schlick(t_computations *comps);

/* -------------------------- Normal Perturbation --------------------------- */
t_tuple			perturb_normal(t_computations *comps);
t_tuple			apply_sphere_tbn(t_computations *comps, t_tuple map_normal);
t_tuple			apply_plane_tbn(t_computations *comps, t_tuple map_normal);
t_tuple			apply_cube_tbn(t_computations *comps, t_tuple map_normal);
t_tuple			apply_cylinder_tbn(t_computations *comps, t_tuple map_normal);
t_tuple			apply_cone_tbn(t_computations *comps, t_tuple map_normal);
t_tuple			apply_torus_tbn(t_computations *comps, t_tuple map_normal);
t_tuple			apply_triangle_tbn(t_computations *comps, t_tuple map_normal);
t_tuple			apply_smooth_triangle_tbn(t_computations *comps,
					t_tuple map_normal);

#endif
