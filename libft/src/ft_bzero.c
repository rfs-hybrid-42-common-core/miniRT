/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 13:25:37 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/14 22:32:20 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn void ft_bzero(void *s, size_t n)
 * @brief Zeroes a byte string.
 * @details Erases the data in the n bytes of the memory starting at the 
 * location pointed to by s, by writing zeros (bytes containing '\0') to that 
 * area.
 * @param s Pointer to the memory area.
 * @param n The number of bytes to zero.
 */
void	ft_bzero(void *s, size_t n)
{
	ft_memset(s, 0, n);
}
