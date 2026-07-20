/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine_free.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 03:31:25 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/12 22:44:31 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "bvh.h"
#include "engine.h"
#include "libft.h"
#include "patterns.h"
#include "shading.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stddef.h>
#include <stdlib.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void free_pattern(t_pattern *pattern, void *mlx_ptr)
 * @brief Deallocates texture buffers and pattern modifiers.
 * @details Crucially tests if an MLX image pointer natively exists inside the 
 * pattern struct. If so, it issues an `mlx_destroy_image` directive to wipe 
 * the memory buffer from the display server before freeing the local structs.
 * @param pattern Pointer to the pattern being erased.
 * @param mlx_ptr The active MLX display server pointer.
 */
static void	free_pattern(t_pattern *pattern, void *mlx_ptr)
{
	int	i;

	if (pattern->type == UV_IMAGE && pattern->u_data.s_uv.texture)
	{
		if (pattern->u_data.s_uv.texture->img_ptr)
			mlx_destroy_image(mlx_ptr, pattern->u_data.s_uv.texture->img_ptr);
		free(pattern->u_data.s_uv.texture);
	}
	else if (pattern->type == UV_SKYBOX)
	{
		i = -1;
		while (++i < 6)
		{
			if (pattern->u_data.s_skybox.faces[i])
			{
				if (pattern->u_data.s_skybox.faces[i]->img_ptr)
					mlx_destroy_image(mlx_ptr,
						pattern->u_data.s_skybox.faces[i]->img_ptr);
				free(pattern->u_data.s_skybox.faces[i]);
			}
		}
	}
	free(pattern);
}

/**
 * @fn static void free_material(t_material *material, void *mlx_ptr)
 * @brief Checks for optional allocated properties attached to a material.
 * @details Material base structs exist locally inside object memory, but their 
 * optional modifiers (bump maps, normal maps, patterns) are dynamically 
 * assigned heap objects. This isolates them for safe deletion.
 * @param material Pointer to the material being evaluated.
 * @param mlx_ptr  The active MLX display server pointer.
 */
static void	free_material(t_material *material, void *mlx_ptr)
{
	if (material->pattern)
		free_pattern(material->pattern, mlx_ptr);
	if (material->normal_map)
		free_pattern(material->normal_map, mlx_ptr);
	if (material->bump_map)
		free_pattern(material->bump_map, mlx_ptr);
}

/**
 * @fn void free_object(t_object *object, void *mlx_ptr)
 * @brief Recursively sweeps and destroys all data tied to an object.
 * @details Cleans the material components first. If the object is a logical 
 * CSG node, it recursively executes itself on the Left and Right physical 
 * children. If it is a logical Group, it iterates through the internal linked 
 * list to recursively delete all child contents before destroying itself.
 * @param object  Pointer to the active object, group, or CSG node.
 * @param mlx_ptr The active MLX display server pointer.
 */
void	free_object(t_object *object, void *mlx_ptr)
{
	t_list	*current;
	t_list	*next;

	if (!object)
		return ;
	free_material(&object->material, mlx_ptr);
	if (object->type == CSG)
	{
		free_object(object->data.csg.left, mlx_ptr);
		free_object(object->data.csg.right, mlx_ptr);
		free(object->data.csg.left_id);
		free(object->data.csg.right_id);
	}
	else if (object->type == GROUP)
	{
		current = object->data.group.children;
		while (current)
		{
			next = current->next;
			free_object((t_object *)current->content, mlx_ptr);
			free(current);
			current = next;
		}
	}
	return (free(object->id), free(object->parent_id), free(object));
}

/**
 * @fn static void free_lights(t_list *lights)
 * @brief Destroys all data inside the global light linked list.
 * @details Sweeps the node, identifying if the light struct is an Area Light. 
 * If so, it safely drops the massively allocated Jitter Sequence array from 
 * the heap before freeing the base list node.
 * @param lights The head node of the global lights list.
 */
static void	free_lights(t_list *lights)
{
	t_light	*light;
	t_list	*next;

	while (lights)
	{
		next = lights->next;
		light = (t_light *)lights->content;
		if (light)
		{
			free(light->jitter);
			free(light);
		}
		free(lights);
		lights = next;
	}
}

/**
 * @fn void free_memory(t_engine *engine)
 * @brief Master memory destruction protocol.
 * @details Systematically destroys all dynamically allocated data to ensure a 
 * clean exit. Clears BIOS linked lists, recursively frees object geometry and 
 * materials, destroys the global BVH structure, purges the lights list, frees 
 * cached engine strings, and safely disconnects the MLX context handlers.
 * @param engine Pointer to the global engine context.
 */
void	free_memory(t_engine *engine)
{
	int	i;

	ft_lstclear(&engine->bios.working_scenes, free);
	ft_lstclear(&engine->bios.broken_scenes, free);
	if (engine->world.objects)
	{
		i = -1;
		while (++i < engine->world.total_objects)
			free_object(engine->world.objects[i], engine->mlx.mlx_ptr);
		free(engine->world.objects);
		engine->world.objects = NULL;
	}
	free_lights(engine->world.lights);
	if (engine->world.bvh && engine->world.bvh->nodes)
		free(engine->world.bvh->nodes);
	free(engine->world.bvh);
	if (engine->mlx.img_ptr)
		mlx_destroy_image(engine->mlx.mlx_ptr, engine->mlx.img_ptr);
	if (engine->mlx.win_ptr)
		mlx_destroy_window(engine->mlx.mlx_ptr, engine->mlx.win_ptr);
	if (engine->mlx.mlx_ptr)
		mlx_destroy_display(engine->mlx.mlx_ptr);
	return (free(engine->mlx.mlx_ptr), free(engine->scene_path),
		free(engine->exec_dir), free(engine->aa_jitter));
}
