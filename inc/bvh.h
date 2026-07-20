/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 14:40:41 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/02 01:06:45 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BVH_H
# define BVH_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "objects.h"
# include "ray.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <stdbool.h>

/* ========================================================================== */
/* MACROS                                                                     */
/* ========================================================================== */

/**
 * @def BVH_MAX_DEPTH
 * @brief Maximum traversal depth for the Bounding Volume Hierarchy tree.
 * @details Used to size the static array stack during iterative BVH traversal 
 * (to prevent recursive stack overflows). 256 is vast enough to handle
 * millions of primitives without ever overflowing.
 */
# ifndef BVH_MAX_DEPTH
#  define BVH_MAX_DEPTH	256
# endif

/* ========================================================================== */
/* DATA STRUCTURES                                                            */
/* ========================================================================== */

/**
 * @struct s_bvh_node
 * @brief A single node within the flat-array BVH tree.
 * @details Represents either a leaf node (containing geometry) or an internal 
 * node (containing child nodes). Uses integer indices instead of pointers to 
 * improve CPU cache locality and prevent memory fragmentation.
 * @var bounds        The Axis-Aligned Bounding Box (AABB) encapsulating this 
 * node.
 * @var first_object  Index of the first object in the flattened world array 
 * (used only if this is a leaf node).
 * @var total_objects Number of objects inside this leaf (0 if internal node).
 * @var left_index    Array index of the left child node.
 * @var right_index   Array index of the right child node.
 */
typedef struct s_bvh_node
{
	t_bounds	bounds;
	int			first_object;
	int			total_objects;
	int			left_index;
	int			right_index;
}	t_bvh_node;

/**
 * @struct s_bvh
 * @brief The main Bounding Volume Hierarchy controller.
 * @details Stores the spatial partitioning tree as a contiguous flat array 
 * of nodes. This setup ensures rapid traversal and O(1) memory cleanup.
 * @var nodes           Pointer to the dynamically allocated flat array of 
 * nodes.
 * @var node_count      The current number of active nodes in the tree.
 * @var nodes_allocated The maximum capacity of the nodes array.
 */
typedef struct s_bvh
{
	t_bvh_node	*nodes;
	int			node_count;
	int			nodes_allocated;
}	t_bvh;

/**
 * @struct s_bvh_range
 * @brief Represents a slice of the object array during BVH construction.
 * @details Used to recursively split the world objects into smaller sub-arrays 
 * based on their spatial median.
 * @var start The starting index of the object slice.
 * @var end   The ending index of the object slice.
 */
typedef struct s_bvh_range
{
	int	start;
	int	end;
}	t_bvh_range;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ---------------------------- BVH Construction ---------------------------- */
t_bvh		*build_bvh(t_object **objects, int total_objects);
int			partition_objects(t_object **objects, t_bvh_range range, int axis);

/* ----------------------------- BVH Traversal ------------------------------ */
void		intersect_bvh(t_bvh *bvh, t_intersections *xs, t_ray *ray,
				t_object **objects);
bool		bvh_is_occluded(t_bvh *bvh, t_ray *ray, double distance,
				t_object **objects);

#endif
