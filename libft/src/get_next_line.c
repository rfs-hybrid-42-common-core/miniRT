/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 16:39:15 by maaugust          #+#    #+#             */
/*   Updated: 2026/06/14 22:37:53 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static char *free_memory(char **ptr)
 * @brief Helper function to safely free a pointer and set it to NULL.
 * @details Consolidates deallocation logic to prevent dangling pointers 
 * and memory leaks across multiple error-handling branches.
 * @param ptr Double pointer to the memory to be freed.
 * @return    Always returns NULL.
 */
static char	*free_memory(char **ptr)
{
	if (ptr && *ptr)
	{
		free(*ptr);
		*ptr = NULL;
	}
	return (NULL);
}

/**
 * @fn static char *ft_str_realloc(char **s, size_t size)
 * @brief Dynamically expands a string to accommodate more characters.
 * @details If the string is NULL, it allocates a new buffer of the given size. 
 * If the string exists, it allocates a larger buffer, copies the old contents, 
 * and frees the previous memory location.
 * @param s    A double pointer to the dynamically allocated string to expand.
 * @param size The additional number of characters the string needs to hold.
 * @return     A pointer to the newly allocated and expanded string, or NULL 
 * if memory allocation fails.
 */
static char	*ft_str_realloc(char **s, size_t size)
{
	char	*new;
	size_t	len;

	if (!s || !*s)
	{
		new = (char *) malloc(sizeof(char) * (size + 1));
		if (!new)
			return (free_memory(s));
		*new = '\0';
		return (new);
	}
	len = ft_strlen(*s);
	new = (char *) malloc(sizeof(char) * (len + size + 1));
	if (!new)
		return (free_memory(s));
	ft_strlcpy(new, *s, len + 1);
	free_memory(s);
	return (new);
}

/**
 * @fn static ssize_t read_line(int fd, char **buffer, char **new_line)
 * @brief Reads data from the file descriptor into a dynamically allocated 
 * buffer.
 * @details Manages heap allocation for the specific file descriptor's buffer. 
 * If reading returns EOF or an error, it cleanly frees the buffer and the 
 * partial line to prevent memory leaks across multiple FDs.
 * @param fd       The file descriptor to read from.
 * @param buffer   Double pointer to the specific FD's residual buffer.
 * @param new_line A double pointer to the dynamically allocated return string.
 * @return         The number of bytes read, 0 if EOF or invalid FD, or -1 on 
 * error.
 */
static ssize_t	read_line(int fd, char **buffer, char **new_line)
{
	ssize_t	n_bytes;

	if (fd < 0 || fd >= FD_SIZE || BUFFER_SIZE <= 0)
		return (0);
	if (!*buffer)
	{
		*buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
		if (!*buffer)
			return (-1);
		**buffer = '\0';
	}
	if (**buffer)
		return (ft_strlen(*buffer));
	n_bytes = read(fd, *buffer, BUFFER_SIZE);
	if (n_bytes > 0)
	{
		(*buffer)[n_bytes] = '\0';
		return (n_bytes);
	}
	if (n_bytes < 0 && *new_line)
		free_memory(new_line);
	free_memory(buffer);
	return (n_bytes);
}

/**
 * @fn char *get_next_line(int fd)
 * @brief Extracts the next line of text from multiple file descriptors.
 * @details Utilizes a static array of pointers to independently track residual 
 * bytes for up to FD_SIZE file descriptors, allowing seamless switching 
 * between files without losing state. Memory is allocated on-demand to 
 * drastically reduce static memory footprint.
 * @param fd The file descriptor to read from.
 * @return   A dynamically allocated string containing the extracted line,
 * or NULL if there is nothing else to read or an error occurs.
 */
char	*get_next_line(int fd)
{
	static char	*buffer[FD_SIZE];
	char		*new_line;
	ssize_t		n_bytes;
	int			i;

	new_line = NULL;
	n_bytes = read_line(fd, &buffer[fd], &new_line);
	while (n_bytes > 0)
	{
		i = 0;
		while (i < n_bytes && buffer[fd][i] && buffer[fd][i++] != '\n')
			;
		new_line = ft_str_realloc(&new_line, i);
		if (!new_line)
			return (free_memory(&buffer[fd]));
		ft_strlcpy(new_line + ft_strlen(new_line), buffer[fd], i + 1);
		ft_strlcpy(buffer[fd], buffer[fd] + i, n_bytes - i + 1);
		if (ft_strchr(new_line, '\n'))
			break ;
		n_bytes = read_line(fd, &buffer[fd], &new_line);
	}
	if (buffer[fd] && buffer[fd][0] == '\0')
		free_memory(&buffer[fd]);
	return (new_line);
}
