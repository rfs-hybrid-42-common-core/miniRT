/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   float_getters.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 23:54:53 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:32:39 by maaugust         ###   ########.fr       */
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
 * @fn double get_fmax(double a, double b)
 * @brief Returns the largest of two floats without library overhead.
 * @details Exists to bypass the <math.h> fmax() function, vastly improving 
 * BVH traversal speed.
 * @param a First value.
 * @param b Second value.
 * @return  The larger of the two values.
 */
double	get_fmax(double a, double b)
{
	if (a > b)
		return (a);
	return (b);
}

/**
 * @fn double get_fmin(double a, double b)
 * @brief Returns the smallest of two floats without library overhead.
 * @details Exists to bypass the <math.h> fmin() function, ensuring rapid 
 * inlining during AABB intersection tests.
 * @param a First value.
 * @param b Second value.
 * @return  The smaller of the two values.
 */
double	get_fmin(double a, double b)
{
	if (a < b)
		return (a);
	return (b);
}
