/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fresnel.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 15:53:24 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 17:57:24 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "shading.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn double schlick(t_computations *comps)
 * @brief Calculates Christophe Schlick's approximation for the Fresnel effect.
 * @details Physically models how transparent objects (like water or glass) 
 * become increasingly mirror-like at grazing viewing angles. If total internal 
 * reflection occurs (sin2_t > 1), it mathematically clamps reflectance to 100%.
 * @param comps Precomputed surface intersection data (n1/n2 indices).
 * @return      The calculated reflectance ratio (0.0 to 1.0).
 */
double	schlick(t_computations *comps)
{
	double	cos;
	double	n_ratio;
	double	sin2_t;
	double	r0;

	cos = dot(comps->eyev, comps->normalv);
	if (comps->n1 > comps->n2 + EPSILON)
	{
		n_ratio = comps->n1 / comps->n2;
		sin2_t = n_ratio * n_ratio * (1.0 - cos * cos);
		if (sin2_t > 1.0 + EPSILON)
			return (1.0);
		cos = sqrt(1.0 - sin2_t);
	}
	r0 = ((comps->n1 - comps->n2) / (comps->n1 + comps->n2))
		* ((comps->n1 - comps->n2) / (comps->n1 + comps->n2));
	return (r0 + (1 - r0) * pow(1 - cos, 5));
}
