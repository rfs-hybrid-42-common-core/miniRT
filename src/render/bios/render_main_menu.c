/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_main_menu.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 22:10:07 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/02 00:31:52 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "render.h"
#include "libft.h"
#include "mlx.h"

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn void render_main_menu(t_engine *engine)
 * @brief Renders the root navigational hub of the BIOS utility.
 * @details Maps out the primary string options into an array. Loops through 
 * the options, evaluating the user's cursor_index to append the active 
 * selection marker and yellow color shift.
 * @param engine The global engine context.
 */
void	render_main_menu(t_engine *engine)
{
	char	*options[5];
	char	buffer[MAX_OPTION_LEN];
	int		i;
	int		c;

	options[0] = "Load Working Scene";
	options[1] = "Test Broken Parsers";
	options[2] = "Parser Reference Manual";
	options[3] = "Camera & Controls";
	options[4] = "Exit miniRT";
	i = -1;
	while (++i < 5)
	{
		c = 0xFFFFFF;
		ft_strlcpy(buffer, "  ", MAX_OPTION_LEN);
		if (i == engine->bios.cursor_index)
		{
			c = 0xFFFF00;
			ft_strlcpy(buffer, "> ", MAX_OPTION_LEN);
		}
		ft_strlcat(buffer, options[i], MAX_OPTION_LEN);
		mlx_string_put(engine->mlx.mlx_ptr, engine->mlx.win_ptr, 100,
			150 + (i * 30), c, buffer);
	}
}
