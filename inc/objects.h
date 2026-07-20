/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   objects.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 00:21:06 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/14 05:52:20 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OBJECTS_H
# define OBJECTS_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "libft.h"
# include "math_rt.h"
# include "patterns.h"
# include "ray.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <stdbool.h>

/* ========================================================================== */
/* MACROS                                                                     */
/* ========================================================================== */

/**
 * @def INFINITY
 * @brief Representation of an infinitely large number.
 * @details Used to initialize empty bounding boxes and define the default 
 * extents of infinite primitives like Planes, Cylinders, and Cones.
 */
# ifndef INFINITY
#  define INFINITY	1e30
# endif

/* ========================================================================== */
/* TYPEDEFS & DATA STRUCTURES                                                 */
/* ========================================================================== */

/**
 * @typedef t_ray
 * @brief Forward declaration of the ray structure.
 * @details Resolves circular dependencies between objects.h and ray.h.
 */
typedef struct s_ray			t_ray;

/**
 * @typedef t_intersection
 * @brief Forward declaration of the intersection structure.
 * @details Resolves circular dependencies between objects.h and ray.h.
 */
typedef struct s_intersection	t_intersection;

/**
 * @typedef t_intersections
 * @brief Forward declaration of the intersections container.
 * @details Resolves circular dependencies between objects.h and ray.h.
 */
typedef struct s_intersections	t_intersections;

/**
 * @typedef t_pattern
 * @brief Forward declaration of the pattern structure.
 * @details Resolves circular dependencies between objects.h and patterns.h.
 */
typedef struct s_pattern		t_pattern;

/**
 * @enum e_object_type
 * @brief Enumeration of all supported geometric primitives and groupings.
 * @details Used to index function pointer arrays (jump tables) to avoid 
 * branching logic during ray intersections and normal calculations.
 * @var SPHERE          A mathematically perfect 3D sphere.
 * @var PLANE           An infinitely extending flat surface.
 * @var CUBE            A 3D Axis-Aligned Bounding Box (AABB) unit cube.
 * @var CYLINDER        A mathematically perfect tube, optionally capped.
 * @var CONE            A double-napped cone, optionally capped.
 * @var TORUS           A 3D donut shape requiring quartic root extraction.
 * @var TRIANGLE        A flat 3-vertex polygon.
 * @var SMOOTH_TRIANGLE A triangle featuring vertex normals for interpolated 
 * shading.
 * @var GROUP           A logical container encapsulating multiple objects.
 * @var CSG             A Constructive Solid Geometry boolean operation node.
 * @var OBJECT_COUNT    The total number of object types, used to size jump 
 * tables.
 */
typedef enum e_object_type
{
	SPHERE,
	PLANE,
	CUBE,
	CYLINDER,
	CONE,
	TORUS,
	TRIANGLE,
	SMOOTH_TRIANGLE,
	GROUP,
	CSG,
	OBJECT_COUNT
}	t_object_type;

/**
 * @struct s_sphere
 * @brief Unique data for a Sphere primitive.
 * @details Natively exists as a unit sphere at (0,0,0) with radius 1.0. 
 * Manipulated purely via its transformation matrix.
 * @var origin The center point of the sphere.
 * @var radius The distance from the center to the surface.
 */
typedef struct s_sphere
{
	t_tuple		origin;
	double		radius;
}	t_sphere;

/**
 * @struct s_cylinder
 * @brief Unique data for a Cylinder primitive.
 * @details Exists as a unit cylinder extending along the Y axis.
 * @var minimum The truncation limit on the negative Y axis.
 * @var maximum The truncation limit on the positive Y axis.
 * @var closed  Boolean flag determining if the ends should render as solid 
 * caps.
 */
typedef struct s_cylinder
{
	double	minimum;
	double	maximum;
	bool	closed;
}	t_cylinder;

/**
 * @struct s_cone
 * @brief Unique data for a Cone primitive.
 * @details Exists as a double-napped cone intersecting at the origin.
 * @var minimum The truncation limit on the negative Y axis.
 * @var maximum The truncation limit on the positive Y axis.
 * @var closed  Boolean flag determining if the ends should render as solid 
 * caps.
 */
typedef struct s_cone
{
	double	minimum;
	double	maximum;
	bool	closed;
}	t_cone;

/**
 * @struct s_torus
 * @brief Unique data for a Torus primitive.
 * @details Represents a donut shape resting on the XZ plane.
 * @var major_r The radius from the origin to the center of the tube.
 * @var minor_r The radius of the tube itself.
 */
typedef struct s_torus
{
	double	major_r;
	double	minor_r;
}	t_torus;

/**
 * @struct s_triangle
 * @brief Unique data for a standard Triangle primitive.
 * @details Precomputes edges and face normals at parse-time to drastically 
 * speed up the Möller–Trumbore intersection algorithm.
 * @var p      Array of 3 tuples representing the absolute vertices.
 * @var e      Array of 2 edge vectors (p1-p0 and p2-p0).
 * @var normal The precomputed geometric normal perpendicular to the face.
 * @var uv     Array of 3 tuples representing 2D texture mapping coordinates.
 */
typedef struct s_triangle
{
	t_tuple	p[3];
	t_tuple	e[2];
	t_tuple	normal;
	t_tuple	uv[3];
}	t_triangle;

/**
 * @struct s_smooth_triangle
 * @brief Unique data for a Smooth Triangle primitive.
 * @details Allows for Phong interpolation across the face using vertex normals 
 * extracted from advanced OBJ files.
 * @var p  Array of 3 tuples representing the absolute vertices.
 * @var e  Array of 2 edge vectors (p1-p0 and p2-p0).
 * @var n  Array of 3 vertex normal vectors.
 * @var uv Array of 3 tuples representing 2D texture mapping coordinates.
 */
typedef struct s_smooth_triangle
{
	t_tuple	p[3];
	t_tuple	e[2];
	t_tuple	n[3];
	t_tuple	uv[3];
}	t_smooth_triangle;

/**
 * @struct s_bounds
 * @brief An Axis-Aligned Bounding Box (AABB).
 * @details Defines the absolute minimum and maximum 3D coordinates that fully 
 * encapsulate an object or group. Used for BVH spatial partitioning.
 * @var min The spatial coordinate containing the lowest X, Y, and Z values.
 * @var max The spatial coordinate containing the highest X, Y, and Z values.
 */
typedef struct s_bounds
{
	t_tuple	min;
	t_tuple	max;
}	t_bounds;

/**
 * @struct s_group
 * @brief Unique data for a Group logical node.
 * @details Acts as an invisible spatial container for multiple objects.
 * @var bounds   The unified AABB bounding box for all children.
 * @var children A linked list of pointers to the encapsulated objects.
 */
typedef struct s_group
{
	t_bounds	bounds;
	t_list		*children;
}	t_group;

/**
 * @enum e_csg_op
 * @brief Enumeration of Constructive Solid Geometry operations.
 * @details Determines how the left and right child nodes logically overlap.
 * @var CSG_UNION        Combines both shapes into a single solid.
 * @var CSG_INTERSECTION Leaves only the overlapping space between the two 
 * shapes.
 * @var CSG_DIFFERENCE   Subtracts the right shape's volume from the left shape.
 * @var TOTAL_CSG_OPS    Total number of valid CSG operations.
 */
typedef enum e_csg_op
{
	CSG_UNION,
	CSG_INTERSECTION,
	CSG_DIFFERENCE,
	TOTAL_CSG_OPS
}	t_csg_op;

/**
 * @struct s_csg
 * @brief Unique data for a CSG (Constructive Solid Geometry) node.
 * @details Applies a boolean mathematical operation to two sub-shapes.
 * @var operation The boolean logic operation to apply 
 * (Union/Intersection/Diff).
 * @var left      Pointer to the primary shape.
 * @var right     Pointer to the secondary shape.
 * @var left_id   The string ID of the left shape (used during parsing).
 * @var right_id  The string ID of the right shape (used during parsing).
 */
typedef struct s_csg
{
	t_csg_op		operation;
	struct s_object	*left;
	struct s_object	*right;
	char			*left_id;
	char			*right_id;
}	t_csg;

/**
 * @union s_object_data
 * @brief Polymorphic container for object-specific properties.
 * @details Allows a generic t_object to represent any primitive type without 
 * relying on unsafe void pointers or fragmented heap allocations. Only the 
 * memory of the actively utilized struct is accessed.
 * @var sphere          Data for SPHERE type.
 * @var cylinder        Data for CYLINDER type.
 * @var cone            Data for CONE type.
 * @var torus           Data for TORUS type.
 * @var triangle        Data for TRIANGLE type.
 * @var smooth_triangle Data for SMOOTH_TRIANGLE type.
 * @var group           Data for GROUP type.
 * @var csg             Data for CSG type.
 */
typedef union s_object_data
{
	t_sphere			sphere;
	t_cylinder			cylinder;
	t_cone				cone;
	t_torus				torus;
	t_triangle			triangle;
	t_smooth_triangle	smooth_triangle;
	t_group				group;
	t_csg				csg;
}	t_object_data;

/**
 * @struct s_material
 * @brief Defines how an object interacts with light.
 * @details Based on the physically-based Blinn-Phong shading model.
 * @var color            The base RGB color of the material.
 * @var pattern          Pointer to a procedural pattern generator or texture.
 * @var normal_map       Pointer to a normal map (for Tangent Space bump data).
 * @var bump_map         Pointer to a height map (for procedural gradients).
 * @var ambient          Ratio of ambient background light absorbed 
 * (0.0 to 1.0).
 * @var diffuse          Ratio of direct diffuse light absorbed (0.0 to 1.0).
 * @var specular         Ratio of direct specular light reflected (0.0 to 1.0).
 * @var shininess        The size of the specular highlight 
 * (higher = smaller/sharper).
 * @var reflective       How mirror-like the surface is (0.0 to 1.0).
 * @var transparency     How much light passes through the material 
 * (0.0 to 1.0).
 * @var refractive_index How much light bends when entering the material 
 * (Snell's Law).
 * @var normal_intensity Multiplier for the extremity of the Tangent Space 
 * normal map.
 * @var bump_strength    Multiplier for the depth of the procedural height map.
 * @var bump_epsilon     The micro-offset used to sample the height map 
 * gradient.
 */
typedef struct s_material
{
	t_color		color;
	t_pattern	*pattern;
	t_pattern	*normal_map;
	t_pattern	*bump_map;
	double		ambient;
	double		diffuse;
	double		specular;
	double		shininess;
	double		reflective;
	double		transparency;
	double		refractive_index;
	double		normal_intensity;
	double		bump_strength;
	double		bump_epsilon;
}	t_material;

/**
 * @struct s_object
 * @brief The universal renderable entity within the scene.
 * @details Holds everything required to place, deform, and shade an object. 
 * Matrices are aggressively precomputed at parse-time to save CPU cycles.
 * @var type                        The shape identifier matching the 
 * object_data union.
 * @var parent                      Pointer to the parent group or CSG node 
 * (if any).
 * @var id                          An optional string identifier for the 
 * object.
 * @var parent_id                   The string identifier of the intended 
 * parent.
 * @var data                        The union holding shape-specific geometric 
 * data.
 * @var transform                   The primary world-space transformation 
 * matrix.
 * @var transform_end               The secondary transformation matrix for 
 * motion blur.
 * @var transform_inverse           Precomputed inverse matrix for local space 
 * conversion.
 * @var transform_inverse_transpose Precomputed matrix for un-skewing surface 
 * normals.
 * @var is_moving                   Flag denoting if the object is 
 * interpolating motion.
 * @var has_shear                   Flag denoting if a shearing transformation 
 * is pending.
 * @var shear_matrix                The temporarily stored shearing matrix 
 * to be baked.
 * @var material                    The surface shading parameters.
 */
typedef struct s_object
{
	t_object_type	type;
	struct s_object	*parent;
	char			*id;
	char			*parent_id;
	t_object_data	data;
	t_matrix		transform;
	t_matrix		transform_end;
	t_matrix		transform_inverse;
	t_matrix		transform_inverse_transpose;
	t_tuple			local_rotation;
	bool			is_moving;
	bool			has_shear;
	t_matrix		shear_matrix;
	t_material		material;
}	t_object;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ------------------------ Initialization & Config ------------------------- */
t_material	create_default_material(void);
void		init_object_defaults(t_object *object);
void		set_object_transform(t_object *object, t_matrix *transform);
void		apply_universal_transform(t_object *object, t_tuple position,
				t_tuple normal, t_tuple scale);
t_object	*create_sphere(void);
t_object	*create_plane(void);
t_object	*create_cube(void);
t_object	*create_cylinder(void);
t_object	*create_cone(void);
t_object	*create_torus(double major_r, double minor_r);
t_object	*create_triangle(t_tuple *p, t_tuple *uv);
t_object	*create_smooth_triangle(t_tuple *p, t_tuple *n, t_tuple *uv);
t_object	*create_group(void);
t_object	*create_csg(t_csg_op operation, t_object *left, t_object *right);
void		add_child(t_object *group, t_object *object);

/* ---------------------- Spatial Transforms & Normals ---------------------- */
t_tuple		world_to_object(t_object *object, t_tuple world_point, double time);
t_tuple		normal_to_world(t_object *object, t_tuple normal, double time);
t_tuple		normal_at(t_intersection *hit, t_tuple world_point, double time);
t_tuple		normal_sphere(t_intersection *hit, t_tuple object_point);
t_tuple		normal_plane(t_intersection *hit, t_tuple object_point);
t_tuple		normal_cube(t_intersection *hit, t_tuple object_point);
t_tuple		normal_cylinder(t_intersection *hit, t_tuple object_point);
t_tuple		normal_cone(t_intersection *hit, t_tuple object_point);
t_tuple		normal_torus(t_intersection *hit, t_tuple object_point);
t_tuple		normal_triangle(t_intersection *hit, t_tuple object_point);
t_tuple		normal_smooth_triangle(t_intersection *hit, t_tuple object_point);

/* ------------------------- Ray Intersection Math -------------------------- */
void		intersect_sphere(t_intersections *xs, t_ray *ray, t_object *object);
void		intersect_plane(t_intersections *xs, t_ray *ray, t_object *object);
void		intersect_cube(t_intersections *xs, t_ray *ray, t_object *object);
void		intersect_cylinder(t_intersections *xs, t_ray *ray,
				t_object *object);
void		intersect_cone(t_intersections *xs, t_ray *ray, t_object *object);
void		intersect_torus(t_intersections *xs, t_ray *ray, t_object *object);
void		intersect_triangle(t_intersections *xs, t_ray *ray,
				t_object *object);
void		intersect_group(t_intersections *xs, t_ray *ray, t_object *object);
void		intersect_csg(t_intersections *xs, t_ray *ray, t_object *object);

/* -------------------------- Bounding Volume Math -------------------------- */
t_bounds	empty_bounding_box(void);
void		add_point_to_bounds(t_bounds *box, t_tuple point);
void		add_box_to_bounds(t_bounds *box1, t_bounds *box2);
t_bounds	bounds_of_sphere(t_object *object);
t_bounds	bounds_of_plane(t_object *object);
t_bounds	bounds_of_cylinder(t_object *object);
t_bounds	bounds_of_cube(t_object *object);
t_bounds	bounds_of_cone(t_object *object);
t_bounds	bounds_of_torus(t_object *object);
t_bounds	bounds_of_triangle(t_object *object);
t_bounds	bounds_of_smooth_triangle(t_object *object);
t_bounds	bounds_of_group(t_object *object);
t_bounds	bounds_of_csg(t_object *object);
t_bounds	bounds_of(t_object *object);
t_bounds	transform_bounds(t_bounds *box, t_matrix *transform);
t_bounds	parent_space_bounds_of(t_object *object);
bool		intersect_bounds(t_ray *ray, t_bounds *bounds);

#endif
