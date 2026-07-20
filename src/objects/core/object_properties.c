/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   object_properties.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 15:15:15 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/15 20:45:45 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "objects.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stddef.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn t_material create_default_material(void)
 * @brief Initializes a material with standard white PBR properties.
 * @details Fills the struct with baseline values (ambient 0.1, diffuse 0.9, 
 * specular 0.9, etc.) to ensure predictable shading if the parser omits 
 * specific material overrides.
 * @return The base material struct.
 */
t_material	create_default_material(void)
{
	return ((t_material){color(1, 1, 1), NULL, NULL, NULL, 0.1, 0.9, 0.0, 0.0,
		0.0, 0.0, 1.0, 1.0, 1.0, 0.0001
	});
}

/**
 * @fn void init_object_defaults(t_object *object)
 * @brief Standardizes the initial memory state of any new object.
 * @details Zeroes out memory to prevent unpredictable behavior, assigns 
 * default materials, explicitly initializes local rotation Euler angles 
 * to zero, sets Identity matrices across the board, and ensures shearing 
 * and motion blur flags are safely initialized to false.
 * @param object Pointer to the newly allocated object.
 */
void	init_object_defaults(t_object *object)
{
	ft_bzero(object, sizeof(t_object));
	object->parent = NULL;
	object->id = NULL;
	object->parent_id = NULL;
	object->transform = identity_matrix();
	object->transform_end = object->transform;
	object->transform_inverse = object->transform;
	object->transform_inverse_transpose = object->transform;
	object->local_rotation = vector(0, 0, 0);
	object->material = create_default_material();
	object->is_moving = false;
	object->has_shear = false;
}

/**
 * @fn void set_object_transform(t_object *object, t_matrix *transform)
 * @brief Applies a transformation matrix to an object.
 * @details Aggressively precomputes the Inverse and Inverse-Transpose matrices 
 * right now at parse-time. This saves millions of heavy O(n!) determinant 
 * calculations during the render phase.
 * @param object    The object to transform.
 * @param transform The transformation matrix.
 */
void	set_object_transform(t_object *object, t_matrix *transform)
{
	object->transform = *transform;
	if (!object->is_moving)
		object->transform_end = object->transform;
	object->transform_inverse = inverse_matrix(&object->transform);
	object->transform_inverse_transpose
		= transpose_matrix(&object->transform_inverse);
}

/**
 * @fn void apply_universal_transform(t_object *object, t_tuple position,
 * t_tuple normal, t_tuple scale_val)
 * @brief Constructs and applies the final affine matrix for any parsed object.
 * @details Multiplies Translation * Base_Alignment * Shear * Local_Rotation 
 * * Scale. Safely incorporates the deferred shearing matrix if the object's 
 * shear flag is active. Condenses matrix caching by aggressively recycling 
 * local variables to perfectly satisfy the 42 Norminette limits.
 * @param object   Pointer to the active object being parsed.
 * @param position The translation coordinates.
 * @param normal   The mandatory alignment vector (Y-axis up).
 * @param scale    The geometric scaling factors.
 */
void	apply_universal_transform(t_object *object, t_tuple position,
	t_tuple normal, t_tuple scale)
{
	t_matrix	scaled;
	t_matrix	aligned;
	t_matrix	rotated;
	t_matrix	translated;
	t_matrix	final;

	scaled = scaling_matrix(scale.x, scale.y, scale.z);
	aligned = align_normal_matrix(normal);
	rotated = combined_rotation_matrix(object->local_rotation);
	translated = translation_matrix(position.x, position.y, position.z);
	final = multiply_matrices(&rotated, &scaled);
	final = multiply_matrices(&aligned, &final);
	if (object->has_shear)
		final = multiply_matrices(&final, &object->shear_matrix);
	scaled = final;
	final = multiply_matrices(&translated, &scaled);
	set_object_transform(object, &final);
	if (object->is_moving)
	{
		translated = translation_matrix(object->transform_end.m[0][3],
				object->transform_end.m[1][3], object->transform_end.m[2][3]);
		object->transform_end = multiply_matrices(&translated, &scaled);
	}
}
