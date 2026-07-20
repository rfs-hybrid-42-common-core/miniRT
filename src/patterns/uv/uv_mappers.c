/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uv_mappers.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 00:45:40 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/29 15:55:00 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "patterns.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn void spherical_map(t_tuple point, t_uv_coord *uv_coord)
 * @brief Wraps a 2D image spherically using polar coordinates.
 * @details Uses atan2 for longitude (U) and acos for latitude (V).
 * @param point    The 3D point on the unit sphere.
 * @param uv_coord The struct to populate with 2D U/V results.
 */
void	spherical_map(t_tuple point, t_uv_coord *uv_coord)
{
	double	val;

	uv_coord->u = 1.0 - (atan2(point.x, point.z) / (2 * M_PI) + 0.5);
	val = point.y / magnitude(vector(point.x, point.y, point.z));
	if (val > 1.0)
		val = 1.0;
	else if (val < -1.0)
		val = -1.0;
	uv_coord->v = 1.0 - acos(val) / M_PI;
}

/**
 * @fn void planar_map(t_tuple point, t_uv_coord *uv_coord)
 * @brief Maps a texture infinitely across a flat plane.
 * @details Employs fmod to tile the texture, dynamically correcting C's 
 * negative modulo quirk to ensure the texture doesn't reverse across zero.
 * @param point    The 3D point on the plane.
 * @param uv_coord The struct to populate.
 */
void	planar_map(t_tuple point, t_uv_coord *uv_coord)
{
	uv_coord->u = fmod(point.x, 1.0);
	if (uv_coord->u < 0)
		uv_coord->u += 1.0;
	uv_coord->v = fmod(point.z, 1.0);
	if (uv_coord->v < 0)
		uv_coord->v += 1.0;
}

/**
 * @fn void cylindrical_map(t_tuple point, t_uv_coord *uv_coord)
 * @brief Maps a texture around a cylinder and its caps.
 * @details Detects if the point lies on the flat caps or the curved wall. 
 * Maps the caps concentrically and wraps the wall using atan2 longitude.
 * @param point    The 3D point on the cylinder.
 * @param uv_coord The struct to populate.
 */
void	cylindrical_map(t_tuple point, t_uv_coord *uv_coord)
{
	double	theta;
	double	radius_squared;

	theta = atan2(point.x, point.z);
	radius_squared = point.x * point.x + point.z * point.z;
	uv_coord->u = 1.0 - ((theta + M_PI) / (2.0 * M_PI));
	if (radius_squared < 1.0 - EPSILON)
		uv_coord->v = fmod(sqrt(radius_squared), 1.0);
	else
	{
		uv_coord->v = fmod(point.y, 1.0);
		if (uv_coord->v < 0)
			uv_coord->v += 1.0;
	}
}

/**
 * @fn void conical_map(t_tuple point, t_uv_coord *uv_coord)
 * @brief Maps a texture onto a cone geometry.
 * @details Uses spatial slant logic for the V coordinate to prevent extreme 
 * texture stretching at the cone's tip.
 * @param point    The 3D point on the cone.
 * @param uv_coord The struct to populate.
 */
void	conical_map(t_tuple point, t_uv_coord *uv_coord)
{
	double	theta;
	double	slant;

	theta = atan2(point.x, point.z);
	slant = sqrt(point.x * point.x + point.y * point.y + point.z * point.z);
	uv_coord->u = 1.0 - ((theta + M_PI) / (2.0 * M_PI));
	uv_coord->v = fmod(slant, 1.0);
}

/**
 * @fn void toroidal_map(t_tuple point, t_uv_coord *uv_coord)
 * @brief Maps a texture perfectly around a Torus ring.
 * @details Converts 3D surface coordinates of a Torus into 2D UV coordinates. 
 * Calculates the major ring angle (theta) for the U axis and the minor tube 
 * angle (phi) for the V axis.
 * @param point    The 3D point on the torus surface.
 * @param uv_coord The struct to populate.
 */
void	toroidal_map(t_tuple point, t_uv_coord *uv_coord)
{
	double	theta;
	double	phi;
	double	tube_center_dist;
	double	pattern_major_r;

	pattern_major_r = 1.0;
	theta = atan2(point.x, point.z);
	tube_center_dist = sqrt(point.x * point.x + point.z * point.z)
		- pattern_major_r;
	phi = atan2(point.y, tube_center_dist);
	uv_coord->u = 1.0 - ((theta + M_PI) / (2.0 * M_PI));
	uv_coord->v = 1.0 - ((phi + M_PI) / (2.0 * M_PI));
}
