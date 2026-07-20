/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   noise.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 23:20:18 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:53:58 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "patterns.h"

/* --------------------------- External Libraries --------------------------- */
#include <stdint.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static double fade(double t)
 * @brief Smooths out coordinate interpolation using a quintic curve.
 * @details Implements Ken Perlin's Improved Noise equation: 
 * 6t^5 - 15t^4 + 10t^3. Prevents visually harsh grid artifacts by ensuring 
 * smooth 1st and 2nd derivatives.
 * @param t The fractional coordinate.
 * @return  The smoothed fractional value.
 */
static double	fade(double t)
{
	return (t * t * t * (t * (t * 6.0 - 15.0) + 10.0));
}

/**
 * @fn static double lerp(double t, double a, double b)
 * @brief Linearly interpolates between two values.
 * @details Solves the standard linear interpolation formula. When t=0, it 
 * returns 'a'. When t=1, it returns 'b'. Crucial for smoothing out noise 
 * gradients.
 * @param t The blend factor (0.0 to 1.0).
 * @param a The starting value.
 * @param b The ending value.
 * @return  The blended result.
 */
static double	lerp(double t, double a, double b)
{
	return (a + t * (b - a));
}

/**
 * @fn static double hash_3d(int x, int y, int z)
 * @brief Generates a deterministic pseudo-random hash for a 3D grid cell.
 * @details Uses bitwise shifts and prime number multiplication to ensure 
 * the random gradients remain locked to the surface coordinates.
 * @param x The grid cell X index.
 * @param y The grid cell Y index.
 * @param z The grid cell Z index.
 * @return  A deterministic random value mapped between 0.0 and 1.0.
 */
static double	hash_3d(int x, int y, int z)
{
	unsigned int	n;

	n = (unsigned int)(x + y * 57 + z * 131);
	n = (n << 13) ^ n;
	return ((double)(n * (n * n * 15731 + 789221) + 1376312589)
			/ (double)INT32_MAX);
}

/**
 * @fn static double perlin_noise(t_tuple pattern_point)
 * @brief Generates one octave of 3D Perlin Noise.
 * @details Identifies the 8 corners of the containing unit cube, calculates 
 * the random hash at each corner, and trilinearly interpolates the values 
 * using the quintic fade curve.
 * @param pattern_point The local 3D coordinate.
 * @return              The smoothed noise value.
 */
static double	perlin_noise(t_tuple pattern_point)
{
	int		integer[3];
	double	frac[3];
	double	t[3];
	double	y[2];

	integer[0] = (int)floor(pattern_point.x);
	integer[1] = (int)floor(pattern_point.y);
	integer[2] = (int)floor(pattern_point.z);
	frac[0] = pattern_point.x - floor(pattern_point.x);
	frac[1] = pattern_point.y - floor(pattern_point.y);
	frac[2] = pattern_point.z - floor(pattern_point.z);
	t[0] = fade(frac[0]);
	t[1] = fade(frac[1]);
	t[2] = fade(frac[2]);
	y[0] = lerp(t[1],
			lerp(t[0], hash_3d(integer[0], integer[1], integer[2]),
				hash_3d(integer[0] + 1, integer[1], integer[2])),
			lerp(t[0], hash_3d(integer[0], integer[1] + 1, integer[2]),
				hash_3d(integer[0] + 1, integer[1] + 1, integer[2])));
	y[1] = lerp(t[1],
			lerp(t[0], hash_3d(integer[0], integer[1], integer[2] + 1),
				hash_3d(integer[0] + 1, integer[1], integer[2] + 1)),
			lerp(t[0], hash_3d(integer[0], integer[1] + 1, integer[2] + 1),
				hash_3d(integer[0] + 1, integer[1] + 1, integer[2] + 1)));
	return (lerp(t[2], y[0], y[1]));
}

/**
 * @fn double turbulence(t_tuple pattern_point, int octaves)
 * @brief Generates fractal noise by summing multiple layers of Perlin noise.
 * @details With each loop (octave), the spatial frequency doubles (details get 
 * smaller) while the amplitude halves (impact gets weaker), creating highly 
 * realistic organic textures.
 * @param pattern_point The local 3D coordinate.
 * @param octaves       The number of noise layers to sum.
 * @return              The final accumulated noise value.
 */
double	turbulence(t_tuple pattern_point, int octaves)
{
	double	accum;
	double	weight;
	t_tuple	tmp;
	int		i;

	accum = 0.0;
	weight = 1.0;
	tmp = pattern_point;
	i = -1;
	while (++i < octaves)
	{
		accum += weight * perlin_noise(tmp);
		weight *= 0.5;
		tmp = scaling_tuple(tmp, 2.0);
	}
	return (fabs(accum));
}
