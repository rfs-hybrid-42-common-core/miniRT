/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   polynomials_quadratic.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:32:03 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:30:20 by maaugust         ###   ########.fr       */
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
 * @fn int solve_quadratic(double *coeffs, double *roots)
 * @brief Solves a standard quadratic equation.
 * @details Uses the quadratic formula to find the roots. Crucial for 
 * intersecting Spheres, Cylinders, and Cones.
 * @param coeffs Array of coefficients [c, b, a].
 * @param roots  Array to store the calculated roots.
 * @return       The number of real roots found (0, 1, or 2).
 */
int	solve_quadratic(double *coeffs, double *roots)
{
	double	v[3];
	double	disc;

	v[0] = coeffs[2];
	v[1] = coeffs[1];
	v[2] = coeffs[0];
	if (fabs(v[0]) < EPSILON)
	{
		if (fabs(v[1]) < EPSILON)
			return (0);
		roots[0] = -v[2] / v[1];
		return (1);
	}
	disc = (v[1] * v[1]) - (4.0 * v[0] * v[2]);
	if (disc < -EPSILON)
		return (0);
	if (disc >= -EPSILON && disc <= EPSILON)
	{
		roots[0] = -v[1] / (2.0 * v[0]);
		return (1);
	}
	roots[0] = (-v[1] - sqrt(disc)) / (2.0 * v[0]);
	roots[1] = (-v[1] + sqrt(disc)) / (2.0 * v[0]);
	return (2);
}
