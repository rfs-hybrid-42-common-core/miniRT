/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   polynomials_quartic.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 17:01:00 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:31:19 by maaugust         ###   ########.fr       */
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
 * @fn static void calc_quartic_terms(double *d_e, double *v, double r, 
 * double z)
 * @brief Precalculates specific algebraic terms for quartic extraction.
 * @details Evaluates the discriminant sub-equations to prepare for the final 
 * root extraction. Handles edge cases when the cubic resolvent R is near zero.
 * @param d_e Array to store the two derived sub-terms.
 * @param v   Array of depressed quartic coefficients.
 * @param r   The dominant root from the resolvent cubic.
 * @param z   The intermediate substitution variable.
 */
static void	calc_quartic_terms(double *d_e, double *v, double r, double z)
{
	double	term;

	if (r < 1e-4)
	{
		term = sqrt(get_fmax(0.0, z * z - 4.0 * v[3]));
		d_e[0] = 0.75 * v[0] * v[0] - 2.0 * v[1] + 2.0 * term;
		d_e[1] = 0.75 * v[0] * v[0] - 2.0 * v[1] - 2.0 * term;
	}
	else
	{
		term = (4.0 * v[0] * v[1] - 8.0 * v[2] - v[0] * v[0] * v[0])
			/ (4.0 * r);
		d_e[0] = 0.75 * v[0] * v[0] - r * r - 2.0 * v[1] + term;
		d_e[1] = 0.75 * v[0] * v[0] - r * r - 2.0 * v[1] - term;
	}
}

/**
 * @fn static int extract_quartic_roots(double *v, double r, double z, 
 * double *roots)
 * @brief Finishes resolving the quartic equation.
 * @details Safely extracts up to 4 real roots depending on the sign of the 
 * precalculated terms.
 * @param v     Array of depressed quartic coefficients.
 * @param r     The resolvent cubic dominant root.
 * @param z     The intermediate variable.
 * @param roots The array to store valid real roots.
 * @return      The total number of real roots found.
 */
static int	extract_quartic_roots(double *v, double r, double z, double *roots)
{
	double	d_e[2];
	int		count;

	calc_quartic_terms(d_e, v, r, z);
	count = 0;
	if (d_e[0] >= -1e-4)
	{
		roots[count++] = -v[0] / 4.0 + r / 2.0
			+ sqrt(get_fmax(0.0, d_e[0])) / 2.0;
		roots[count++] = -v[0] / 4.0 + r / 2.0
			- sqrt(get_fmax(0.0, d_e[0])) / 2.0;
	}
	if (d_e[1] >= -1e-4)
	{
		roots[count++] = -v[0] / 4.0 - r / 2.0
			+ sqrt(get_fmax(0.0, d_e[1])) / 2.0;
		roots[count++] = -v[0] / 4.0 - r / 2.0
			- sqrt(get_fmax(0.0, d_e[1])) / 2.0;
	}
	return (count);
}

/**
 * @fn int solve_quartic(double *coeffs, double *roots)
 * @brief Solves a quartic (4th degree) equation algebraically.
 * @details Uses the Resolvent Cubic method. Crucial for finding ray 
 * intersections with mathematically complex surfaces, specifically the Torus.
 * @param coeffs Array of the [e, d, c, b, a] coefficients.
 * @param roots  Array to store the valid real roots.
 * @return       The total number of real roots found (0 to 4).
 */
int	solve_quartic(double *coeffs, double *roots)
{
	double	v[4];
	double	cb[4];
	double	z[3];
	double	r;
	int		total_roots;

	if (fabs(coeffs[4]) < EPSILON)
		return (solve_cubic(coeffs, roots));
	v[0] = coeffs[3] / coeffs[4];
	v[1] = coeffs[2] / coeffs[4];
	v[2] = coeffs[1] / coeffs[4];
	v[3] = coeffs[0] / coeffs[4];
	cb[3] = 1.0;
	cb[2] = -v[1];
	cb[1] = v[0] * v[2] - 4.0 * v[3];
	cb[0] = 4.0 * v[1] * v[3] - v[0] * v[0] * v[3] - v[2] * v[2];
	total_roots = solve_cubic(cb, z);
	if (total_roots == 0)
		return (0);
	if (total_roots == 2)
		z[0] = get_fmax(z[0], z[1]);
	else if (total_roots == 3)
		z[0] = get_fmax(z[0], get_fmax(z[1], z[2]));
	r = sqrt(get_fmax(0.0, 0.25 * v[0] * v[0] - v[1] + z[0]));
	return (extract_quartic_roots(v, r, z[0], roots));
}
