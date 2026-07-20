/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_islower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 22:30:20 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/14 22:32:46 by maaugust         ###   ########.fr       */
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
 * @fn static int ft_islower(int c)
 * @brief Checks if a character is a lowercase letter.
 * @details Evaluates whether the passed integer corresponds to a lowercase 
 * letter in the ASCII table ('a' through 'z').
 * @param c The character to test.
 * @return  1 if the character is lowercase, 0 otherwise.
 */
int	ft_islower(int c)
{
	return (c >= 'a' && c <= 'z');
}
