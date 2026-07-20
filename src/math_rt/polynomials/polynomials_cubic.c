/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   polynomials_cubic.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 16:44:43 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:29:59 by maaugust         ###   ########.fr       */
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
 * @fn static double cubic_single_root(double x)
 * @brief Safely computes the cube root of a single real number.
 * @details The standard pow(x, 1.0/3.0) function evaluates to NaN if x is 
 * negative. This wrapper extracts the root safely by flipping the sign 
 * manually.
 * @param x The operand.
 * @return  The safe cubic root.
 */
static double	cubic_single_root(double x)
{
	if (x < 0.0)
		return (-pow(-x, 1.0 / 3.0));
	return (pow(x, 1.0 / 3.0));
}

/**
 * @fn static int cubic_multi_roots(double *v, double disc, double *roots)
 * @brief Extracts roots from a depressed cubic with multiple real roots.
 * @details Evaluates the discriminant. If the depressed equation indicates 
 * 3 distinct real roots, it uses trigonometric substitution (acos/cos) to 
 * find them. Otherwise, solves algebraically for 1 or 2 roots.
 * @param v     Array of depressed cubic terms.
 * @param disc  The calculated discriminant.
 * @param roots The array to store the found roots.
 * @return      The number of valid real roots found.
 */
static int	cubic_multi_roots(double *v, double disc, double *roots)
{
	double	sq_p;
	double	theta;

	if (disc < -EPSILON)
	{
		sq_p = sqrt(-v[0] / 3.0);
		theta = acos(get_fmax(-1.0,
					get_fmin(1.0, -v[1] / (2.0 * sq_p * sq_p * sq_p))));
		roots[0] = 2.0 * sq_p * cos(theta / 3.0) - v[2];
		roots[1] = 2.0 * sq_p * cos((theta + 2.0 * M_PI) / 3.0) - v[2];
		roots[2] = 2.0 * sq_p * cos((theta + 4.0 * M_PI) / 3.0) - v[2];
		return (3);
	}
	if (fabs(v[0]) < EPSILON && fabs(v[1]) < EPSILON)
	{
		roots[0] = -v[2];
		return (1);
	}
	roots[0] = (3.0 * v[1]) / v[0] - v[2];
	roots[1] = -(3.0 * v[1]) / (2.0 * v[0]) - v[2];
	return (2);
}

/**
 * @fn int solve_cubic(double *coeffs, double *roots)
 * @brief Solves a cubic equation algebraically.
 * @details Depresses the cubic equation (eliminating the squared term) to 
 * analyze the discriminant. Falls back to solve_quadratic if the cubic 
 * coefficient is essentially zero.
 * @param coeffs The array containing the [d, c, b, a] coefficients.
 * @param roots  The array to store the valid real roots.
 * @return       The number of real roots found.
 */
int	solve_cubic(double *coeffs, double *roots)
{
	double	v[3];
	double	disc;
	double	u[2];

	if (fabs(coeffs[3]) < EPSILON)
		return (solve_quadratic(coeffs, roots));
	v[0] = (3.0 * (coeffs[1] / coeffs[3])
			- ((coeffs[2] / coeffs[3]) * (coeffs[2] / coeffs[3]))) / 3.0;
	v[1] = (2.0 * ((coeffs[2] * coeffs[2] * coeffs[2])
				/ (coeffs[3] * coeffs[3] * coeffs[3]))
			- 9.0 * (coeffs[2] / coeffs[3]) * (coeffs[1] / coeffs[3])
			+ 27.0 * (coeffs[0] / coeffs[3])) / 27.0;
	v[2] = (coeffs[2] / coeffs[3]) / 3.0;
	disc = (v[1] * v[1]) / (2.0 * 2.0)
		+ (v[0] * v[0] * v[0]) / (3.0 * 3.0 * 3.0);
	if (disc > EPSILON)
	{
		u[0] = cubic_single_root(-v[1] / 2.0 + sqrt(disc));
		u[1] = cubic_single_root(-v[1] / 2.0 - sqrt(disc));
		roots[0] = u[0] + u[1] - v[2];
		return (1);
	}
	return (cubic_multi_roots(v, disc, roots));
}
