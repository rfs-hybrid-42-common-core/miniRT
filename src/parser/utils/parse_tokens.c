/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/26 04:35:04 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 03:16:09 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "parser.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn void extract_token(const char **ptr, char *buffer, int max_len)
 * @brief Copies a string chunk bounded by whitespaces.
 * @details Skips leading spaces, copies characters until hitting the next 
 * space, and actively truncates execution to avoid buffer overruns. 
 * Null-terminates the resulting buffer string.
 * @param ptr     Double pointer to the raw string.
 * @param buffer  The destination character array.
 * @param max_len The absolute maximum permitted length of the extraction.
 */
void	extract_token(const char **ptr, char *buffer, int max_len)
{
	int	i;

	i = 0;
	while (ft_isspace(**ptr))
		(*ptr)++;
	while (**ptr && !ft_isspace(**ptr) && i < max_len - 1)
		buffer[i++] = *(*ptr)++;
	buffer[i] = '\0';
}

/**
 * @fn void skip_token(const char **ptr)
 * @brief Jumps the pointer past a contiguous string block.
 * @details Used to bypass standard type identifiers (e.g., 'sp', 'pl') to 
 * align the pointer squarely at the mathematical arguments.
 * @param ptr Double pointer to the raw string.
 */
void	skip_token(const char **ptr)
{
	while (**ptr && !ft_isspace(**ptr))
		(*ptr)++;
	while (ft_isspace(**ptr))
		(*ptr)++;
}

/**
 * @fn void advance_comma(const char **ptr, bool *error)
 * @brief Enforces the presence of a comma delimiter.
 * @details Skips optional whitespace. If the direct next character is not a 
 * comma, it immediately flips the global parsing error flag to false.
 * @param ptr   Double pointer to the raw string.
 * @param error Pointer to the global error boolean.
 */
void	advance_comma(const char **ptr, bool *error)
{
	while (ft_isspace(**ptr))
		(*ptr)++;
	if (**ptr == ',')
		(*ptr)++;
	else
		*error = true;
}
