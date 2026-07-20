/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 13:32:04 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/14 22:32:23 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stdint.h>
#include <stdlib.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn void *ft_calloc(size_t nmemb, size_t size)
 * @brief Allocates memory for an array and initializes it to zero.
 * @details Allocates memory for an array of nmemb elements of size bytes each 
 * and returns a pointer to the allocated memory. The memory is set to zero. 
 * Includes a check for integer overflow.
 * @param nmemb Number of elements.
 * @param size  Size of each element.
 * @return      Pointer to the allocated memory, or NULL if allocation fails.
 */
void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*ptr;
	size_t	total_size;

	if (size && nmemb > SIZE_MAX / size)
		return (NULL);
	total_size = nmemb * size;
	ptr = malloc(total_size);
	if (ptr)
		ft_bzero(ptr, total_size);
	return (ptr);
}
