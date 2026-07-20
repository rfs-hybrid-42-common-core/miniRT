/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tuples_arithmetics.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 15:12:55 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 02:38:24 by maaugust         ###   ########.fr       */
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
 * @fn t_tuple add_tuples(t_tuple t1, t_tuple t2)
 * @brief Adds two tuples component-wise.
 * @details Adding a vector to a point (w=0 + w=1) results in a new point. 
 * Adding two vectors (w=0 + w=0) results in a combined vector.
 * @param t1 The first tuple.
 * @param t2 The second tuple.
 * @return   The resultant tuple.
 */
t_tuple	add_tuples(t_tuple t1, t_tuple t2)
{
	return ((t_tuple){t1.x + t2.x, t1.y + t2.y, t1.z + t2.z, t1.w + t2.w});
}

/**
 * @fn t_tuple subtract_tuples(t_tuple t1, t_tuple t2)
 * @brief Subtracts the second tuple from the first component-wise.
 * @details Subtracting two points (w=1 - w=1) yields a vector (w=0) pointing 
 * from the second to the first. Subtracting a vector from a point yields a 
 * point.
 * @param t1 The base tuple.
 * @param t2 The tuple to subtract.
 * @return   The resultant tuple.
 */
t_tuple	subtract_tuples(t_tuple t1, t_tuple t2)
{
	return ((t_tuple){t1.x - t2.x, t1.y - t2.y, t1.z - t2.z, t1.w - t2.w});
}

/**
 * @fn t_tuple scaling_tuple(t_tuple tuple, double n)
 * @brief Multiplies a tuple by a scalar value.
 * @details Used to stretch a vector (increase magnitude) or scale a point.
 * @param tuple The tuple to scale.
 * @param n     The scalar multiplier.
 * @return      The scaled tuple.
 */
t_tuple	scaling_tuple(t_tuple tuple, double n)
{
	return ((t_tuple){tuple.x * n, tuple.y * n, tuple.z * n, tuple.w * n});
}

/**
 * @fn t_tuple negating_tuple(t_tuple tuple)
 * @brief Flips the sign of every component in a tuple.
 * @details Reverses a vector so it points in the exact opposite direction.
 * @param tuple The tuple to negate.
 * @return      The negated tuple.
 */
t_tuple	negating_tuple(t_tuple tuple)
{
	return ((t_tuple){-tuple.x, -tuple.y, -tuple.z, -tuple.w});
}
