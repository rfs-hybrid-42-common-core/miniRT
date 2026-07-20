/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_toupper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 13:30:32 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/14 22:35:41 by maaugust         ###   ########.fr       */
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
 * @fn int ft_toupper(int c)
 * @brief Converts a lowercase letter to uppercase.
 * @details If the passed character is a lowercase letter, it converts it to 
 * its uppercase equivalent by subtracting the ASCII offset difference.
 * @param c The character to convert.
 * @return  The corresponding uppercase letter if c is lowercase; 
 * otherwise, the argument is returned unchanged.
 */
int	ft_toupper(int c)
{
	if (ft_islower(c))
		c -= 'a' - 'A';
	return (c);
}
