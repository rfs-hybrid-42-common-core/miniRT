/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 13:32:39 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/14 22:35:32 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stdlib.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn char *ft_substr(char const *s, unsigned int start, size_t len)
 * @brief Allocates and returns a substring from the string 's'.
 * @details The substring begins at index 'start' and is of maximum size 'len'.
 * @param s     The string from which to create the substring.
 * @param start The start index of the substring in the string 's'.
 * @param len   The maximum length of the substring.
 * @return      The substring, or NULL if allocation fails.
 */
char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	s_len;
	size_t	finish;

	if (!s)
		return (NULL);
	s_len = ft_strlen(s);
	if (start >= s_len)
		return (ft_strdup(""));
	finish = s_len - start;
	if (finish > len)
		finish = len;
	sub = malloc(finish + 1);
	if (!sub)
		return (NULL);
	ft_strlcpy(sub, s + start, finish + 1);
	return (sub);
}
