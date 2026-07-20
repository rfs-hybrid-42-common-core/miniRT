/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   math_rt.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 14:22:39 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/03 14:26:50 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MATH_RT_H
# define MATH_RT_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- Internal Libraries --------------------------- */
# include <math.h>
# include <stdbool.h>
# include <stdint.h>

/* ========================================================================== */
/* MACROS                                                                     */
/* ========================================================================== */

/**
 * @def M_PI
 * @brief The mathematical constant Pi.
 * @details Defined manually as a fallback in case the compiler environment 
 * does not provide it in <math.h>.
 */
# ifndef M_PI
#  define M_PI		3.14159265358979323846
# endif

/**
 * @def EPSILON
 * @brief A minute floating-point tolerance value.
 * @details Used to absorb floating-point precision loss during intersection 
 * tests and to prevent shadow acne by slightly offsetting ray origins.
 */
# ifndef EPSILON
#  define EPSILON	1e-8
# endif

/* ========================================================================== */
/* DATA STRUCTURES                                                            */
/* ========================================================================== */

/**
 * @struct s_tuple
 * @brief A 4D vector representing a point or vector in 3D space.
 * @details Uses a homogeneous coordinate `w`. If `w=1.0`, it is a point (can 
 * be translated). If `w=0.0`, it is a vector (only represents 
 * direction/magnitude).
 * @var x The X coordinate.
 * @var y The Y coordinate.
 * @var z The Z coordinate.
 * @var w The homogeneous w-component.
 */
typedef struct s_tuple
{
	double	x;
	double	y;
	double	z;
	double	w;
}	t_tuple;

/**
 * @struct s_color
 * @brief Represents an RGB color using floating-point math.
 * @details Values are normalized between 0.0 and 1.0 (instead of 0-255) to 
 * allow for easy additive light blending and intensity scaling.
 * @var r The Red channel (0.0 to 1.0).
 * @var g The Green channel (0.0 to 1.0).
 * @var b The Blue channel (0.0 to 1.0).
 */
typedef struct s_color
{
	double	r;
	double	g;
	double	b;
}	t_color;

/**
 * @struct s_matrix
 * @brief A 4x4 Transformation Matrix.
 * @details Used to translate, rotate, and scale points and vectors in 3D space.
 * @var m A 2D array of doubles representing the 4x4 grid.
 */
typedef struct s_matrix
{
	double	m[4][4];
}	t_matrix;

/**
 * @struct s_shear
 * @brief Container for shearing (skew) transformations.
 * @var xy Shears X in proportion to Y.
 * @var xz Shears X in proportion to Z.
 * @var yx Shears Y in proportion to X.
 * @var yz Shears Y in proportion to Z.
 * @var zx Shears Z in proportion to X.
 * @var zy Shears Z in proportion to Y.
 */
typedef struct s_shear
{
	double	xy;
	double	xz;
	double	yx;
	double	yz;
	double	zx;
	double	zy;
}	t_shear;

/**
 * @struct s_quat
 * @brief A Quaternion used for smooth 3D rotations.
 * @details Prevents gimbal lock and scaling artifacts during interpolation 
 * (used for motion blur).
 * @var w The real/scalar part.
 * @var x The i imaginary part.
 * @var y The j imaginary part.
 * @var z The k imaginary part.
 */
typedef struct s_quat
{
	double	w;
	double	x;
	double	y;
	double	z;
}	t_quat;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ---------------------------- Tuples & Vectors ---------------------------- */
t_tuple		point(double x, double y, double z);
t_tuple		vector(double x, double y, double z);
t_tuple		add_tuples(t_tuple t1, t_tuple t2);
t_tuple		subtract_tuples(t_tuple t1, t_tuple t2);
t_tuple		scaling_tuple(t_tuple tuple, double n);
t_tuple		negating_tuple(t_tuple tuple);
double		dot(t_tuple t1, t_tuple t2);
t_tuple		cross(t_tuple t1, t_tuple t2);
double		magnitude(t_tuple tuple);
t_tuple		normalize(t_tuple tuple);
t_tuple		reflect(t_tuple in, t_tuple normal);

/* --------------------------------- Colors --------------------------------- */
t_color		color(double r, double g, double b);
t_color		add_colors(t_color c1, t_color c2);
t_color		subtract_colors(t_color c1, t_color c2);
t_color		multiply_colors(t_color c1, t_color c2);
t_color		scaling_color(t_color c, double n);
t_color		interpolation_color(t_color c1, t_color c2, double t);

/* ---------------------------- Matrices (Core) ----------------------------- */
t_matrix	create_matrix(void);
t_matrix	identity_matrix(void);
bool		equal_matrices(t_matrix *m1, t_matrix *m2);
t_matrix	transpose_matrix(t_matrix *matrix);
t_matrix	multiply_matrices(t_matrix *m1, t_matrix *m2);
t_tuple		multiply_matrix_tuple(t_matrix *matrix, t_tuple tuple);
t_matrix	align_normal_matrix(t_tuple normal);

/* -------------------------- Matrices (Inversion) -------------------------- */
t_matrix	submatrix(t_matrix *matrix, int del_row, int del_col, int size);
double		minor(t_matrix *matrix, int row, int col, int size);
double		cofactor(t_matrix *matrix, int row, int col, int size);
double		determinant(t_matrix *matrix, int size);
t_matrix	inverse_matrix(t_matrix *matrix);

/* ----------------------- Matrices (Transformations) ----------------------- */
t_matrix	translation_matrix(double x, double y, double z);
t_matrix	scaling_matrix(double x, double y, double z);
t_matrix	shearing_matrix(t_shear *shear);
t_matrix	rotation_matrix_x(double rad);
t_matrix	rotation_matrix_y(double rad);
t_matrix	rotation_matrix_z(double rad);
t_matrix	combined_rotation_matrix(t_tuple rotation);

/* ---------------------- Quaternions & Interpolation ----------------------- */
t_quat		matrix_to_quat(t_matrix *matrix);
t_matrix	quat_to_matrix(t_quat *quat);
t_quat		slerp_quat(t_quat q1, t_quat q2, double t);
t_matrix	interpolate_transform(t_matrix *start, t_matrix *end, double t);

/* --------------------------- Polynomial Solvers --------------------------- */
int			solve_quadratic(double *coeffs, double *roots);
int			solve_cubic(double *coeffs, double *roots);
int			solve_quartic(double *coeffs, double *roots);

/* ----------------------------- Math Utilities ----------------------------- */
double		get_fmin(double a, double b);
double		get_fmax(double a, double b);
double		random_double(uint32_t *rng);

#endif
