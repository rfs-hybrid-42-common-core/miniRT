/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_validators.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 15:31:40 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/12 22:41:00 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"
#include "parser.h"

/* --------------------------- External Libraries --------------------------- */
#include <stddef.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn bool validate_vector_range(t_tuple vector)
 * @brief Evaluates an extracted orientation vector.
 * @details Checks if the X, Y, and Z parameters sit exclusively between 
 * -1.0 and 1.0 according to subject rules.
 * @param vector The parsed 3D vector.
 * @return       True if the range is valid, false otherwise.
 */
bool	validate_vector_range(t_tuple vector)
{
	if (vector.x < -1.0 || vector.x > 1.0
		|| vector.y < -1.0 || vector.y > 1.0
		|| vector.z < -1.0 || vector.z > 1.0)
		return (false);
	return (true);
}

/**
 * @fn bool has_trailing_garbage(const char *ptr)
 * @brief Ensures a parsed line does not end with unexpected parameters.
 * @details Skips trailing spaces. The line is considered 'clean' only if it 
 * successfully resolves to a null terminator, a newline, or the '#' comment 
 * tag.
 * @param ptr Pointer to the tail end of the parsed string line.
 * @return    True if invalid characters remain, false if clear.
 */
bool	has_trailing_garbage(const char *ptr)
{
	while (ft_isspace(*ptr))
		ptr++;
	if (*ptr != '\0' && *ptr != '\n' && *ptr != '#')
		return (true);
	return (false);
}

/**
 * @fn bool get_filepath(const char **ptr, char *dest, char delimeter)
 * @brief Extracts a string bound by custom brackets/brackets.
 * @details Used to extract paths like `bump:[textures/normal.xpm]` where the 
 * path is terminated by a specific closing delimiter (like a bracket or comma) 
 * rather than simple whitespace.
 * @param ptr       Double pointer to the raw string block.
 * @param dest      The destination character array.
 * @param delimeter The stopping character.
 * @return          True if the extraction failed (empty string), false if 
 * successful.
 */
bool	get_filepath(const char **ptr, char *dest, char delimeter)
{
	int	i;

	i = 0;
	while (ft_isspace(**ptr))
		(*ptr)++;
	while (**ptr && **ptr != delimeter && !ft_isspace(**ptr)
		&& i < MAX_PATH_LEN - 1)
		dest[i++] = *(*ptr)++;
	dest[i] = '\0';
	if (**ptr == delimeter)
		(*ptr)++;
	return (i == 0);
}

/**
 * @fn bool has_xpm_extension(const char *path)
 * @brief Validates a file extension formatting string.
 * @details Triggers string length calculations to ensure the extracted 
 * file path strictly terminates with the ".xpm" designator to prevent MLX 
 * image loader crashes.
 * @param path The literal string path to evaluate.
 * @return     True if the extension matches exactly, false otherwise.
 */
bool	has_xpm_extension(const char *path)
{
	size_t	len;

	if (!path)
		return (false);
	len = ft_strlen(path);
	if (len < 4)
		return (false);
	return (!ft_strncmp(path + len - 4, ".xpm", 4));
}
