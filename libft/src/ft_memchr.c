/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 13:31:20 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/14 22:33:37 by maaugust         ###   ########.fr       */
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
 * @fn void *ft_memchr(const void *s, int c, size_t n)
 * @brief Scans memory for a character.
 * @details Scans the initial n bytes of the memory area pointed to by s 
 * for the first instance of c. Both c and the bytes of s are interpreted as 
 * unsigned char.
 * @param s The memory area to scan.
 * @param c The character to search for.
 * @param n The number of bytes to scan.
 * @return  A pointer to the matching byte or NULL if the character does not 
 * occur.
 */
void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*ptr;

	ptr = (const unsigned char *)s;
	while (n--)
	{
		if (*ptr == (unsigned char)c)
			return ((void *)ptr);
		ptr++;
	}
	return (NULL);
}
