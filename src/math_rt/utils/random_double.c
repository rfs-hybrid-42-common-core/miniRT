/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   random_double.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 20:34:42 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:33:05 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "math_rt.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn double random_double(uint32_t *rng)
 * @brief Generates a random double between 0.0 and 1.0.
 * @details Uses a Custom Linear Congruential Generator (LCG). Unlike standard 
 * rand(), this is perfectly Thread-Safe because it uses a locally passed state 
 * pointer instead of hidden global memory. Crucial for multithreaded 
 * anti-aliasing.
 * @param rng Pointer to the thread's local random state (seed).
 * @return    A randomized float between 0.0 and 1.0.
 */
double	random_double(uint32_t *rng)
{
	*rng = *rng * 1103515245 + 12345;
	return ((double)((unsigned int)(*rng / 65536) % 32768) / 32768.0);
}
