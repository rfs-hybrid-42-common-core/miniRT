/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 13:33:49 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/14 22:34:15 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stdbool.h>
#include <stdlib.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static char **free_split(char **split, size_t i)
 * @brief Frees a previously allocated array of strings in case of a malloc 
 * failure.
 * @details Iterates backwards from the given index 'i', freeing each string, 
 * and finally frees the main array pointer itself to prevent memory leaks.
 * @param split The double pointer array to free.
 * @param i The number of successfully allocated strings before the failure.
 * @return  Always returns NULL to be passed back by the main function.
 */
static char	**free_split(char **split, size_t i)
{
	while (i > 0)
		free(split[--i]);
	free(split);
	return (NULL);
}

/**
 * @fn static size_t count_words(char const *s, char c)
 * @brief Counts the total number of words in a string separated by a delimiter.
 * @details Iterates through the string, identifying word boundaries by 
 * checking for transitions between delimiter and non-delimiter characters.
 * @param s The string to be analyzed.
 * @param c The delimiter character.
 * @return  The total number of words found.
 */
static size_t	count_words(char const *s, char c)
{
	size_t	count;
	bool	in_word;

	count = 0;
	in_word = false;
	while (*s)
	{
		if (*s != c && !in_word)
		{
			in_word = true;
			count++;
		}
		else if (*s == c)
			in_word = false;
		s++;
	}
	return (count);
}

/**
 * @fn static char *get_next_word(char const **s, char c)
 * @brief Extracts the next word from the string sequence.
 * @details Advances the string pointer past any leading delimiters, calculates 
 * the length of the upcoming word, and creates a substring. The original 
 * string pointer is updated via double indirection to point past the extracted 
 * word.
 * @param s A pointer to the string pointer being parsed.
 * @param c The delimiter character.
 * @return  A newly allocated string containing the extracted word.
 */
static char	*get_next_word(char const **s, char c)
{
	char	*word;
	size_t	len;

	while (**s == c)
		(*s)++;
	len = 0;
	while ((*s)[len] && (*s)[len] != c)
		len++;
	word = ft_substr(*s, 0, len);
	*s += len;
	return (word);
}

/**
 * @fn char **ft_split(char const *s, char c)
 * @brief Splits a string using a delimiter character.
 * @details Allocates and returns an array of strings obtained by splitting 's' 
 * using the character 'c' as a delimiter. The array ends with a NULL pointer.
 * @param s The string to split.
 * @param c The delimiter character.
 * @return  The array of new strings resulting from the split, or NULL if 
 * allocation fails.
 */
char	**ft_split(char const *s, char c)
{
	char	**split;
	size_t	n_words;
	size_t	i;

	if (!s)
		return (NULL);
	n_words = count_words(s, c);
	split = malloc(sizeof(char *) * (n_words + 1));
	if (!split)
		return (NULL);
	i = 0;
	while (i < n_words)
	{
		split[i] = get_next_word(&s, c);
		if (!split[i])
			return (free_split(split, i));
		i++;
	}
	split[n_words] = NULL;
	return (split);
}
