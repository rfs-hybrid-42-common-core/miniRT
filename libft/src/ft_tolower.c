/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 13:30:43 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/14 22:35:38 by maaugust         ###   ########.fr       */
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
 * @fn int ft_tolower(int c)
 * @brief Converts an uppercase letter to lowercase.
 * @details If the passed character is an uppercase letter, it converts it to 
 * its lowercase equivalent by adding the ASCII offset difference.
 * @param c The character to convert.
 * @return  The corresponding lowercase letter if c is uppercase; 
 * otherwise, the argument is returned unchanged.
 */
int	ft_tolower(int c)
{
	if (ft_isupper(c))
		c += 'a' - 'A';
	return (c);
}
