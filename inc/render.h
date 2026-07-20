/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 21:50:58 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/03 19:25:20 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RENDER_H
# define RENDER_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "engine.h"
# include "math_rt.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <pthread.h>

/* ========================================================================== */
/* MACROS                                                                     */
/* ========================================================================== */

/**
 * @def MAX_OPTION_LEN
 * @brief Maximum string length for a BIOS menu option.
 * @details Prevents buffer overflows when formatting the cursor UI strings 
 * using strlcat and strlcpy.
 */
# ifndef MAX_OPTION_LEN
#  define MAX_OPTION_LEN		256
# endif

/**
 * @def MAX_RECURSION
 * @brief Maximum depth for recursive ray bounces.
 * @details Prevents infinite loops when two mirrors face each other. A depth 
 * of 5 is the industry standard for capturing realistic internal reflections 
 * without exponential performance decay.
 */
# ifndef MAX_RECURSION
#  define MAX_RECURSION			5
# endif

/**
 * @def NUM_THREADS
 * @brief Total number of concurrent POSIX threads to spawn.
 * @details Hardcoded to 8 to optimally saturate the logical cores on standard 
 * evaluation environment hardware (e.g., iMacs or Linux VMs).
 */
# ifndef NUM_THREADS
#  define NUM_THREADS			8
# endif

/**
 * @def TILE_SIZE
 * @brief Dimension of a single rendering block (100x100 pixels).
 * @details Used to partition the screen into a workload grid. 100x100 
 * provides a perfect balance between thread fetching overhead and granular
 * load balancing.
 */
# ifndef TILE_SIZE
#  define TILE_SIZE				100
# endif

/**
 * @def PERFORMANCE_STEP
 * @brief Resolution downscaling factor for real-time movement.
 * @details Determines the pixel skipping interval when Performance Mode is 
 * active. A value of 4 means the engine calculates 1 pixel and draws a 4x4 
 * solid block over the buffer, reducing the total ray count by nearly 94% 
 * to achieve smooth viewport translation without ghosting artifacts.
 */
# ifndef PERFORMANCE_STEP
#  define PERFORMANCE_STEP		4
# endif

/* ========================================================================== */
/* DATA STRUCTURES                                                            */
/* ========================================================================== */

/**
 * @struct s_tile
 * @brief Defines the screen boundary for a specific workload chunk.
 * @details Rather than rendering scanline by scanline, threads render in 
 * localized grids to maximize CPU cache coherency and memory locality.
 * @var start_x The starting X pixel coordinate.
 * @var start_y The starting Y pixel coordinate.
 * @var end_x   The ending X pixel coordinate.
 * @var end_y   The ending Y pixel coordinate.
 */
typedef struct s_tile
{
	int	start_x;
	int	start_y;
	int	end_x;
	int	end_y;
}	t_tile;

/**
 * @struct s_render_pool
 * @brief Thread-safe workload manager.
 * @details Houses the complete array of screen tiles. Threads constantly query 
 * this pool, locking the mutex, stealing the next available tile index, and 
 * unlocking it. This guarantees 100% CPU saturation and perfect load balancing.
 * @var engine      Pointer to the global engine state.
 * @var tiles       Dynamically allocated array of tile workload bounds.
 * @var total_tiles The total number of tiles generated for the resolution.
 * @var tile_index  The current global progress counter.
 * @var tile_mutex  POSIX mutex preventing race conditions during index 
 * fetching.
 */
typedef struct s_render_pool
{
	t_engine		*engine;
	t_tile			*tiles;
	int				total_tiles;
	int				tile_index;
	pthread_mutex_t	tile_mutex;
}	t_render_pool;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* --------------------------- BIOS UI Rendering ---------------------------- */
void	render_bios(t_engine *engine);
void	render_main_menu(t_engine *engine);
void	render_file_list(t_engine *engine, t_list *list);
void	render_docs_parser(t_engine *engine);
void	render_docs_controls(t_engine *engine);

/* --------------------------- Raytracer Pipeline --------------------------- */
void	render_scene(t_engine *engine);
void	render_tile_pixelated(t_engine *engine, t_tile *tile, int step);
void	render_tile(t_engine *engine, t_tile *tile);
t_color	sample_pixel(t_engine *engine, int x, int y);

#endif
