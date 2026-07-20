/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jitter.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 14:56:28 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 17:53:13 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "shading.h"

/* --------------------------- External Libraries --------------------------- */
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn double *generate_jitter_sequence(int count)
 * @brief Precomputes an array of random offsets.
 * @details Allocated once during area light construction. Seeds a custom LCG 
 * with the system microsecond clock to populate the sequence. This guarantees 
 * high-performance pseudo-randomness during multi-threaded rendering.
 * @param count The required size of the array.
 * @return      Pointer to the dynamically allocated sequence array.
 */
double	*generate_jitter_sequence(int count)
{
	double			*seq;
	struct timeval	tv;
	uint32_t		rng;
	int				i;

	seq = malloc(sizeof(double) * count);
	if (!seq)
		return (NULL);
	gettimeofday(&tv, NULL);
	rng = tv.tv_usec;
	i = -1;
	while (++i < count)
		seq[i] = random_double(&rng);
	return (seq);
}

/**
 * @fn double get_jitter(t_light *light, t_tuple point, int u, int v)
 * @brief Retrieves a deterministic jitter offset for an area light sample.
 * @details Employs a 3D spatial hashing algorithm combined with the area 
 * light's internal U/V cell indices. This ensures the random noise pattern 
 * remains locked to the specific 3D coordinate, preventing shadows from 
 * "swimming" or vibrating across the surface.
 * @param light The area light providing the jitter array.
 * @param point The 3D surface coordinate being shadowed.
 * @param u     The horizontal sub-cell being sampled.
 * @param v     The vertical sub-cell being sampled.
 * @return      A deterministic random float.
 */
double	get_jitter(t_light *light, t_tuple point, int u, int v)
{
	int	index;
	int	ix;
	int	iy;
	int	iz;

	ix = (int)(point.x * 10000.0);
	iy = (int)(point.y * 10000.0);
	iz = (int)(point.z * 10000.0);
	if (ix < 0)
		ix = -ix;
	if (iy < 0)
		iy = -iy;
	if (iz < 0)
		iz = -iz;
	index = ix * 31 + iy * 73 + iz * 101 + u * 137 + v * 191;
	index = index % light->jitter_size;
	return (light->jitter[index]);
}
