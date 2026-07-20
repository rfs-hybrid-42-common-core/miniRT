/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine_close.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 14:50:04 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/12 22:40:20 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "libft.h"
#include "engine.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <stdlib.h>
#include <unistd.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn void print_error(char *error_msg, char *filepath)
 * @brief Outputs standardized formatted errors to Standard Error (FD 2).
 * @details Adheres strictly to the 42 Subject rules mandating "Error\n" as 
 * the first printed message. Appends optional context like specific broken 
 * file paths.
 * @param error_msg The literal explanation string to print.
 * @param filepath  Optional string holding an offending file path.
 */
void	print_error(char *error_msg, char *filepath)
{
	ft_putendl_fd("Error", STDERR_FILENO);
	ft_putstr_fd(error_msg, STDERR_FILENO);
	if (filepath)
	{
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(filepath, STDERR_FILENO);
	}
	ft_putchar_fd('\n', STDERR_FILENO);
}

/**
 * @fn int exit_program(t_engine *engine)
 * @brief Global termination intercept.
 * @details Catches both manual quits (X button, Esc Key) and catastrophic 
 * failures (Malloc, Parsing). Executes the master memory cleanup routine 
 * before issuing an OS-level exit with the assigned status code. If RT_OPEN 
 * fails early, it bypasses cleanup as memory was not yet allocated.
 * @param engine Pointer to the global engine context.
 * @return       Mandatory integer return to appease MLX hook requirements.
 */
int	exit_program(t_engine *engine)
{
	free_memory(engine);
	exit(engine->exit_code);
	return (EXIT_SUCCESS);
}
