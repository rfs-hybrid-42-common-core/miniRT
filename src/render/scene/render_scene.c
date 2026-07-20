/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_scene.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 23:15:35 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/02 22:18:01 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "camera.h"
#include "libft.h"
#include "mlx.h"
#include "render.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void set_tile_bounds(t_render_pool *pool, t_camera *cam)
 * @brief Populates the workload array with pixel coordinates.
 * @details Iterates over the camera's resolution, defining standard blocks 
 * (e.g., 100x100). Safely clamps the final edges to the true screen resolution 
 * so tiles don't request out-of-bounds pixel generation on odd resolutions.
 * @param pool The active render pool.
 * @param cam  The camera holding the viewport resolution.
 */
static void	set_tile_bounds(t_render_pool *pool, t_camera *cam)
{
	int	x;
	int	y;
	int	i;

	i = 0;
	y = 0;
	while (y < cam->vsize)
	{
		x = 0;
		while (x < cam->hsize)
		{
			pool->tiles[i].start_x = x;
			pool->tiles[i].start_y = y;
			pool->tiles[i].end_x = x + TILE_SIZE;
			if (pool->tiles[i].end_x > cam->hsize)
				pool->tiles[i].end_x = cam->hsize;
			pool->tiles[i].end_y = y + TILE_SIZE;
			if (pool->tiles[i].end_y > cam->vsize)
				pool->tiles[i].end_y = cam->vsize;
			i++;
			x += TILE_SIZE;
		}
		y += TILE_SIZE;
	}
}

/**
 * @fn static void generate_tiles(t_render_pool *pool, t_camera *cam)
 * @brief Allocates the global tile array based on screen resolution.
 * @details Divides the total width and height by the tile size. Uses integer 
 * math with a standard ceiling trick `(+ TILE_SIZE - 1) / TILE_SIZE` to ensure 
 * the remainder bounds are allocated a tile.
 * @param pool The render pool to initialize.
 * @param cam  The camera holding the resolution.
 */
static void	generate_tiles(t_render_pool *pool, t_camera *cam)
{
	int	total_colums;
	int	total_rows;

	total_rows = (cam->vsize + TILE_SIZE - 1) / TILE_SIZE;
	total_colums = (cam->hsize + TILE_SIZE - 1) / TILE_SIZE;
	pool->total_tiles = total_rows * total_colums;
	pool->tiles = malloc(sizeof(t_tile) * pool->total_tiles);
	if (!pool->tiles)
	{
		ft_putendl_fd("Error\nMalloc failed for render tiles", STDERR_FILENO);
		exit_program(pool->engine);
	}
	set_tile_bounds(pool, cam);
	pool->tile_index = 0;
}

/**
 * @fn static void *display_tiles(void *arg)
 * @brief The infinite thread-worker loop.
 * @details Each thread safely locks the mutex, pulls the current tile index, 
 * increments it, unlocks the mutex, and renders the tile autonomously. 
 * Evaluates the engine's render flags to dynamically route the execution to 
 * either the standard renderer or the high-speed pixelated pass. This loop 
 * terminates smoothly when the global index surpasses the total tile count.
 * @param arg A void pointer to the t_render_pool struct.
 * @return    NULL upon successful termination.
 */
static void	*display_tiles(void *arg)
{
	t_render_pool	*pool;
	t_tile			current_tile;

	pool = (t_render_pool *)arg;
	while (1)
	{
		pthread_mutex_lock(&pool->tile_mutex);
		if (pool->tile_index >= pool->total_tiles)
		{
			pthread_mutex_unlock(&pool->tile_mutex);
			break ;
		}
		current_tile = pool->tiles[pool->tile_index];
		pool->tile_index++;
		pthread_mutex_unlock(&pool->tile_mutex);
		if (pool->engine->render_flags & FLAG_PIXELATED_MODE)
			render_tile_pixelated(pool->engine, &current_tile,
				PERFORMANCE_STEP);
		else
			render_tile(pool->engine, &current_tile);
	}
	return (NULL);
}

/**
 * @fn static bool launch_threads(t_render_pool *pool, pthread_t *threads)
 * @brief Spawns the required number of POSIX threads.
 * @details Includes aggressive error-handling. If thread creation fails midway 
 * (e.g., OS resource limit), it artificially maxes out the tile_index to force 
 * the already active threads to finish gracefully before safely aborting.
 * @param pool    The fully allocated render pool.
 * @param threads The stack-allocated array holding the thread IDs.
 * @return        True if all threads launched successfully, false otherwise.
 */
static bool	launch_threads(t_render_pool *pool, pthread_t *threads)
{
	int	i;

	i = -1;
	while (++i < NUM_THREADS)
	{
		if (pthread_create(&threads[i], NULL, display_tiles, pool) != 0)
		{
			pthread_mutex_lock(&pool->tile_mutex);
			pool->tile_index = pool->total_tiles;
			pthread_mutex_unlock(&pool->tile_mutex);
			while (--i >= 0)
				pthread_join(threads[i], NULL);
			free(pool->tiles);
			return (false);
		}
	}
	return (true);
}

/**
 * @fn void render_scene(t_engine *engine)
 * @brief Master controller for multithreaded raytracing.
 * @details Initializes the rendering pool, launches the threads, and forces 
 * the main UI thread to block (via pthread_join) until all worker threads 
 * finish. Once complete, it pushes the fully painted buffer to the MLX window.
 * @param engine The global engine context.
 */
void	render_scene(t_engine *engine)
{
	t_render_pool	pool;
	pthread_t		threads[NUM_THREADS];
	int				i;

	pool.engine = engine;
	generate_tiles(&pool, &engine->camera);
	if (pthread_mutex_init(&pool.tile_mutex, NULL) != 0)
	{
		free(pool.tiles);
		ft_putendl_fd("Error\nFailed to initialize tile mutex", STDERR_FILENO);
		return (engine->exit_code = THREAD, (void)exit_program(engine));
	}
	if (!launch_threads(&pool, threads))
	{
		pthread_mutex_destroy(&pool.tile_mutex);
		ft_putendl_fd("Error\nFailed to launch render threads", STDERR_FILENO);
		return (engine->exit_code = THREAD, (void)exit_program(engine));
	}
	i = -1;
	while (++i < NUM_THREADS)
		pthread_join(threads[i], NULL);
	pthread_mutex_destroy(&pool.tile_mutex);
	free(pool.tiles);
	mlx_put_image_to_window(engine->mlx.mlx_ptr, engine->mlx.win_ptr,
		engine->mlx.img_ptr, 0, 0);
}
