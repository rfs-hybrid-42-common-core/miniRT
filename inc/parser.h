/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 15:26:52 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/03 15:52:17 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "engine.h"
# include "math_rt.h"
# include "objects.h"
# include "shading.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <stdbool.h>
# include <stdint.h>

/* ========================================================================== */
/* MACROS                                                                     */
/* ========================================================================== */

/**
 * @def TAG_AP
 * @brief Bitmask flag for the Aperture camera modifier tag.
 * @details Used in a uint16_t flag tracker to safely catch duplicate tags.
 */
# ifndef TAG_AP
#  define TAG_AP				0x1
# endif

/**
 * @def TAG_FOCAL
 * @brief Bitmask flag for the Focal Length camera modifier tag.
 * @details Used to prevent the user from defining multiple focal points.
 */
# ifndef TAG_FOCAL
#  define TAG_FOCAL				0x2
# endif

/**
 * @def TAG_TYPE
 * @brief Bitmask flag for the Light Type modifier tag.
 * @details Prevents a light from being declared as both a Point and an Area 
 * light.
 */
# ifndef TAG_TYPE
#  define TAG_TYPE				0x1
# endif

/**
 * @def TAG_DIR
 * @brief Bitmask flag for the Light Direction modifier tag.
 * @details Tracks if a Spotlight target direction vector was provided.
 */
# ifndef TAG_DIR
#  define TAG_DIR				0x2
# endif

/**
 * @def TAG_IN
 * @brief Bitmask flag for the Spotlight Inner Cutoff modifier tag.
 * @details Tracks if the inner cone radius angle was provided.
 */
# ifndef TAG_IN
#  define TAG_IN				0x4
# endif

/**
 * @def TAG_OUT
 * @brief Bitmask flag for the Spotlight Outer Cutoff modifier tag.
 * @details Tracks if the outer cone fade angle was provided.
 */
# ifndef TAG_OUT
#  define TAG_OUT				0x8
# endif

/**
 * @def TAG_USTEP
 * @brief Bitmask flag for the Area Light Horizontal Steps modifier tag.
 * @details Tracks if the U-axis sub-cell count was explicitly set.
 */
# ifndef TAG_USTEP
#  define TAG_USTEP				0x10
# endif

/**
 * @def TAG_VSTEP
 * @brief Bitmask flag for the Area Light Vertical Steps modifier tag.
 * @details Tracks if the V-axis sub-cell count was explicitly set.
 */
# ifndef TAG_VSTEP
#  define TAG_VSTEP				0x20
# endif

/**
 * @def TAG_UVEC
 * @brief Bitmask flag for the Area Light Horizontal Width modifier tag.
 * @details Tracks if the U-axis physical width vector was provided.
 */
# ifndef TAG_UVEC
#  define TAG_UVEC				0x40
# endif

/**
 * @def TAG_VVEC
 * @brief Bitmask flag for the Area Light Vertical Height modifier tag.
 * @details Tracks if the V-axis physical height vector was provided.
 */
# ifndef TAG_VVEC
#  define TAG_VVEC				0x80
# endif

/**
 * @def TAG_MAT
 * @brief Bitmask flag for the Material properties modifier tag.
 * @details Tracks if a custom `mat:[ambient],[diffuse],[specular],[shininess]` 
 * override was applied to the active geometry.
 */
# ifndef TAG_MAT
#  define TAG_MAT				0x1
# endif

/**
 * @def TAG_REF
 * @brief Bitmask flag for the Reflective modifier tag.
 * @details Tracks if the `ref:[ratio]` mirror tag was applied.
 */
# ifndef TAG_REF
#  define TAG_REF				0x2
# endif

/**
 * @def TAG_GLASS
 * @brief Bitmask flag for the Glass (Refractive) modifier tag.
 * @details Tracks if the `glass:[ior],[transparency]` tag was applied.
 */
# ifndef TAG_GLASS
#  define TAG_GLASS				0x4
# endif

/**
 * @def TAG_PAT
 * @brief Bitmask flag for Procedural Texture Pattern modifier tags.
 * @details Prevents an object from simultaneously having conflicting 
 * patterns (e.g., trying to apply both a checkerboard and a skybox).
 */
# ifndef TAG_PAT
#  define TAG_PAT				0x8
# endif

/**
 * @def TAG_BUMP
 * @brief Bitmask flag for the Bump Map modifier tag.
 * @details Tracks if a `bump:[file],[intensity],[epsilon]` tag was applied.
 */
# ifndef TAG_BUMP
#  define TAG_BUMP				0x10
# endif

/**
 * @def TAG_ROT
 * @brief Bitmask flag ensuring the 'rot:' tag is only applied once per object.
 * @details Used within the parser's tag_flags tracker. If a parsed object 
 * attempts to define local Euler rotations multiple times, this flag 
 * triggers the abort sequence to prevent silent overwrites.
 */
# ifndef TAG_ROT
#  define TAG_ROT               0x20
# endif

/**
 * @def TAG_SHR
 * @brief Bitmask flag for the Shearing modifier tag.
 * @details Tracks if a `shr:[xy],[xz],[yx],[yz],[zx],[zy]` tag was applied to 
 * skew the geometry.
 */
# ifndef TAG_SHR
#  define TAG_SHR               0x40
# endif

/**
 * @def TAG_MOV
 * @brief Bitmask flag for the Motion Blur modifier tag.
 * @details Tracks if a `mov:[x,y,z]` destination vector was applied.
 */
# ifndef TAG_MOV
#  define TAG_MOV				0x80
# endif

/**
 * @def TAG_CAPS
 * @brief Bitmask flag for the Geometric Caps modifier tag.
 * @details Tracks if a `caps:[0/1]` override was explicitly set on a 
 * Cylinder or Cone primitive.
 */
# ifndef TAG_CAPS
#  define TAG_CAPS				0x100
# endif

/**
 * @def TAG_ID
 * @brief Bitmask flag for the Object Identification modifier tag.
 * @details Tracks if an `id:[name]` tag was applied for CSG grouping.
 */
# ifndef TAG_ID
#  define TAG_ID				0x200
# endif

/**
 * @def TAG_PARENT
 * @brief Bitmask flag for the Parent Hierarchy modifier tag.
 * @details Tracks if a `parent:[name]` tag was applied to attach the object 
 * to a defined group.
 */
# ifndef TAG_PARENT
#  define TAG_PARENT			0x400
# endif

/**
 * @def MAX_FLOAT_LEN
 * @brief Maximum character limit for parsing floating point strings.
 * @details Security limit during atof_advance to prevent buffer overflow 
 * reading massive, infinitely long floating point typos.
 */
# ifndef MAX_FLOAT_LEN
#  define MAX_FLOAT_LEN			30
# endif

/**
 * @def MAX_TAG_LEN
 * @brief Maximum character length of a modifier key tag.
 * @details Limits the parsing buffer for keys like `mat:` or `glass:`.
 */
# ifndef MAX_TAG_LEN
#  define MAX_TAG_LEN			16
# endif

/**
 * @def MAX_PATH_LEN
 * @brief Maximum allowed file path length for textures and OBJs.
 * @details Secures the parsing buffer against excessively deep subdirectories.
 */
# ifndef MAX_PATH_LEN
#  define MAX_PATH_LEN			1024
# endif

/**
 * @def MAX_MODIFIER_LEN
 * @brief Maximum total character length of a modifier's argument block.
 * @details Limits the parsing array when extracting strings like `[file.xpm]`.
 */
# ifndef MAX_MODIFIER_LEN
#  define MAX_MODIFIER_LEN		256
# endif

/**
 * @def RES_WIDTH_DEFAULT
 * @brief Fallback camera resolution (horizontal).
 * @details Triggered if the RT file omits the mandatory 'R' resolution tag.
 */
# ifndef RES_WIDTH_DEFAULT
#  define RES_WIDTH_DEFAULT		800
# endif

/**
 * @def RES_HEIGHT_DEFAULT
 * @brief Fallback camera resolution (vertical).
 * @details Triggered if the RT file omits the mandatory 'R' resolution tag.
 */
# ifndef RES_HEIGHT_DEFAULT
#  define RES_HEIGHT_DEFAULT	600
# endif

/**
 * @def LIGHT_IN_CUT_DEFAULT
 * @brief Default Spotlight inner cone angle (in degrees).
 * @details Evaluated if the user defines a Spot light but omits the 'in:' tag.
 */
# ifndef LIGHT_IN_CUT_DEFAULT
#  define LIGHT_IN_CUT_DEFAULT	15.0
# endif

/**
 * @def LIGHT_OUT_CUT_DEFAULT
 * @brief Default Spotlight outer cone angle (in degrees).
 * @details Evaluated if the user defines a Spot light but omits the 'out:' tag.
 */
# ifndef LIGHT_OUT_CUT_DEFAULT
#  define LIGHT_OUT_CUT_DEFAULT	30.0
# endif

/**
 * @def LIGHT_USTEPS_DEFAULT
 * @brief Default horizontal sampling cells for an Area Light.
 * @details Evaluated if the user omits the 'ustep:' tag.
 */
# ifndef LIGHT_USTEPS_DEFAULT
#  define LIGHT_USTEPS_DEFAULT	4
# endif

/**
 * @def LIGHT_VSTEPS_DEFAULT
 * @brief Default vertical sampling cells for an Area Light.
 * @details Evaluated if the user omits the 'vstep:' tag.
 */
# ifndef LIGHT_VSTEPS_DEFAULT
#  define LIGHT_VSTEPS_DEFAULT	4
# endif

/* ========================================================================== */
/* TYPEDEFS & DATA STRUCTURES                                                 */
/* ========================================================================== */

/**
 * @struct s_file
 * @brief Optimized file reading controller.
 * @details Replaces the highly inefficient get_next_line with a custom, 
 * massive block-reading buffer (UINT16_MAX). Drastically accelerates parsing 
 * for gigantic files (e.g., 200MB OBJ meshes) by eliminating redundant 
 * operating system read() calls.
 * @var fd        The active file descriptor.
 * @var buf       The massive static character buffer array.
 * @var buf_pos   The current index tracking read progress within the buffer.
 * @var buf_bytes The total number of valid bytes returned by the last read() 
 * call.
 */
typedef struct s_file
{
	int		fd;
	char	buf[UINT16_MAX + 1];
	int		buf_pos;
	int		buf_bytes;
}	t_file;

/**
 * @struct s_rt_parser
 * @brief Master state container for the RT scene parser.
 * @details Holds the engine context alongside local validation flags to ensure 
 * the file strictly adheres to the subject rules before allowing the render.
 * @var engine         Pointer to the global engine state.
 * @var file           The fast-read block controller.
 * @var tag_flags      Bitmask tracking which optional modifiers were applied.
 * @var objects        The linked list accumulating valid parsed objects.
 * @var has_resolution Boolean tracking if the 'R' tag was found.
 * @var has_ambient    Boolean tracking if the 'A' tag was found.
 * @var has_camera     Boolean tracking if the 'C' tag was found.
 */
typedef struct s_rt_parser
{
	t_engine	*engine;
	t_file		file;
	uint16_t	tag_flags;
	t_list		*objects;
	bool		has_resolution;
	bool		has_ambient;
	bool		has_camera;
}	t_rt_parser;

/**
 * @struct s_parse_route
 * @brief Function pointer routing node for the RT parser.
 * @details Used to map 2-character string identifiers (e.g., "sp") to their 
 * respective extraction functions without writing massive if-else blocks.
 * @var id         The string identifier (e.g., "sp", "pl", "cy").
 * @var parse_func Pointer to the specific parsing function.
 */
typedef struct s_parse_route
{
	char	*id;
	void	(*parse_func)(const char *, t_rt_parser *);
}	t_parse_route;

/**
 * @struct s_obj_parser
 * @brief Master state container for the OBJ Mesh parser.
 * @details Designed to handle massive triangular geometry files. Employs a 
 * two-pass counting system to pre-allocate flat arrays for vertices, UVs, and 
 * Normals before spawning the triangle primitives.
 * @var rt_parser     Pointer back to the main RT parsing state.
 * @var file          The fast-read block controller for the .obj file.
 * @var main_group    The root group encapsulating the entire mesh.
 * @var current_group Sub-group tracker for handling distinct 'g' tags in OBJs.
 * @var v_array       Dynamically allocated array of raw geometric vertices (v).
 * @var vt_array      Dynamically allocated array of texture mapping 
 * coordinates (vt).
 * @var vn_array      Dynamically allocated array of explicit vertex normals 
 * (vn).
 * @var v_count       Total counted vertices for pre-allocation.
 * @var v_index       Current index during the array population phase.
 * @var vt_count      Total counted texture coords for pre-allocation.
 * @var vt_index      Current index during the array population phase.
 * @var vn_count      Total counted vertex normals for pre-allocation.
 * @var vn_index      Current index during the array population phase.
 */
typedef struct s_obj_parser
{
	t_rt_parser	*rt_parser;
	t_file		file;
	t_object	*main_group;
	t_object	*current_group;
	t_tuple		*v_array;
	t_tuple		*vt_array;
	t_tuple		*vn_array;
	int			v_count;
	int			v_index;
	int			vt_count;
	int			vt_index;
	int			vn_count;
	int			vn_index;
}	t_obj_parser;

/**
 * @struct s_obj_face_index
 * @brief Temporary tracker for decoding an OBJ face parameter.
 * @details Deconstructs the 'f 1/2/3' string block into its raw integer 
 * indices for vertices (v), texture coordinates (vt), and normals (vn).
 * @var v  The raw integer index of the geometric vertex.
 * @var vt The raw integer index of the texture mapping coordinate.
 * @var vn The raw integer index of the explicit vertex normal.
 */
typedef struct s_obj_face_index
{
	int	v;
	int	vt;
	int	vn;
}	t_obj_face_index;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ----------------------------- Master Parsing ----------------------------- */
void		parse_rt_file(char *filepath, t_engine *engine);
t_object	*parse_obj_file(char *filepath, t_rt_parser *rt_parser);
void		link_object_hierarchy(t_rt_parser *parser);

/* -------------------------- Environment Parsing --------------------------- */
void		parse_resolution(const char *ptr, t_rt_parser *parser);
void		parse_ambient(const char *ptr, t_rt_parser *parser);
void		parse_camera(const char *ptr, t_rt_parser *parser);
void		parse_light(const char *ptr, t_rt_parser *parser);

/* --------------------------- Light Sub-Parsing ---------------------------- */
void		parse_light_tag_type(t_light *light, const char **ptr,
				t_rt_parser *parser);
void		parse_light_tag_direction(t_light *light, const char **ptr,
				t_rt_parser *parser);
void		parse_light_tag_cutoff(t_light *light, const char *key,
				const char **ptr, t_rt_parser *parser);
void		parse_light_tag_steps(t_light *light, const char *key,
				const char **ptr, t_rt_parser *parser);
void		parse_light_tag_vectors(t_light *light, const char *key,
				const char **ptr, t_rt_parser *parser);

/* --------------------------- Primitive Parsing ---------------------------- */
void		parse_sphere(const char *ptr, t_rt_parser *parser);
void		parse_plane(const char *ptr, t_rt_parser *parser);
void		parse_cube(const char *ptr, t_rt_parser *parser);
void		parse_cylinder(const char *ptr, t_rt_parser *parser);
void		parse_cone(const char *ptr, t_rt_parser *parser);
void		parse_torus(const char *ptr, t_rt_parser *parser);
void		parse_triangle(const char *ptr, t_rt_parser *parser);
void		parse_smooth_triangle(const char *ptr, t_rt_parser *parser);
void		parse_obj(const char *ptr, t_rt_parser *parser);
void		parse_group(const char *ptr, t_rt_parser *parser);
void		parse_csg(const char *ptr, t_rt_parser *parser);

/* ----------------------------- Modifier Tags ------------------------------ */
void		parse_object_tags(t_object *object, const char *ptr,
				t_rt_parser *parser);
void		parse_material_tags(t_object *obj, const char *key,
				const char **ptr, t_rt_parser *parser);
void		parse_procedural_tags(t_object *object, const char *key,
				const char **ptr, t_rt_parser *parser);
void		parse_texture_tags(t_object *object, const char *key,
				const char **ptr, t_rt_parser *parser);
t_uv_map	get_uv_map(t_object *object, t_rt_parser *parser);
void		parse_img_map(t_object *object, const char **ptr,
				t_rt_parser *parser);
void		parse_bump_map(t_object *object, const char **ptr,
				t_rt_parser *parser);
void		parse_transform_tags(t_object *object, const char *key,
				const char **ptr, t_rt_parser *parser);
void		parse_modifier_tags(t_object *object, const char *key,
				const char **ptr, t_rt_parser *parser);
void		parse_environment_tags(t_object *object, const char *key,
				const char **ptr, t_rt_parser *parser);

/* ---------------------------- OBJ Sub-Parsing ----------------------------- */
void		parse_v(char *line, t_obj_parser *parser);
void		parse_vt(char *line, t_obj_parser *parser);
void		parse_vn(char *line, t_obj_parser *parser);
void		parse_f(char *line, t_obj_parser *parser);
void		parse_g(char *line, t_obj_parser *parser);

/* ------------------------- Abort & Memory Freeing ------------------------- */
void		abort_rt_parse(t_rt_parser *parser, char *error_msg,
				t_exit_code exit_code);
void		abort_obj_parse(t_obj_parser *parser, char *error_msg,
				char *filepath, t_exit_code exit_code);

/* --------------------------- Memory Allocation ---------------------------- */
void		append_light(t_light *light, t_rt_parser *parser);
void		append_object(t_object *object, t_rt_parser *parser);

/* ----------------------------- Block Reading ------------------------------ */
bool		fast_read_line(char *line, t_file *file);

/* ---------------------------- Math Extraction ----------------------------- */
int			atoi_advance(const char **nptr, bool *error, bool comma_allowed);
double		atof_advance(const char **nptr, bool *error, bool comma_allowed);
t_tuple		parse_tuple(const char **ptr, bool *error, double w);
t_color		parse_color(const char **ptr, bool *error);
t_tuple		parse_normal(const char **ptr, bool *error);

/* ---------------------------- String Utilities ---------------------------- */
void		advance_comma(const char **ptr, bool *error);
void		extract_token(const char **ptr, char *buffer, int max_len);
void		skip_token(const char **ptr);
bool		validate_vector_range(t_tuple vector);
bool		has_trailing_garbage(const char *ptr);
bool		get_filepath(const char **ptr, char *dest, char delimeter);
bool		has_xpm_extension(const char *path);

#endif
