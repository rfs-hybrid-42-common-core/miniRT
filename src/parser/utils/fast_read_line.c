/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fast_read_line.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 15:31:40 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/30 02:59:46 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"
#include "parser.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <unistd.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn bool fast_read_line(char *line, t_file *file)
 * @brief High-performance buffered line reader.
 * @details The standard get_next_line (GNL) relies on allocating memory for 
 * every single line, reading character-by-character, and returning strings. 
 * This creates a massive memory fragmentation bottleneck on large files (e.g., 
 * 5,000,000 vertex OBJ files). This function bypasses allocation entirely by 
 * copying data directly from a gigantic 64KB static buffer straight into the 
 * provided stack-allocated array.
 * @param line Pointer to the stack-allocated string buffer ready to receive 
 * data.
 * @param file The fast-read controller struct containing the massive buffer.
 * @return     True if a line was successfully extracted, false if EOF is 
 * reached.
 */
bool	fast_read_line(char *line, t_file *file)
{
	int	i;

	i = 0;
	while (1)
	{
		if (file->buf_pos >= file->buf_bytes)
		{
			file->buf_bytes = read(file->fd, file->buf, UINT16_MAX + 1);
			file->buf_pos = 0;
			if (file->buf_bytes <= 0)
				break ;
		}
		line[i++] = file->buf[file->buf_pos++];
		if (file->buf[file->buf_pos - 1] == '\n' || i >= BUFFER_SIZE - 1)
			break ;
	}
	line[i] = '\0';
	return (i > 0);
}
