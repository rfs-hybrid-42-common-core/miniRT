/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_file_list.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 22:12:23 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/02 00:31:39 by maaugust         ###   ########.fr       */
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
 * @fn static void print_file_item(t_engine *engine, char *name, int i, 
 * int drawn)
 * @brief Prints an individual filename directly to the UI.
 * @details Evaluates the position of the engine's cursor. If the cursor index 
 * matches the item index, it injects a selector carrot (>) and shifts the text 
 * color to vibrant yellow.
 * @param engine The global engine context.
 * @param name   The string filename.
 * @param i      The master index of the file in the list.
 * @param drawn  The vertical offset index for UI spacing.
 */
static void	print_file_item(t_engine *engine, char *name, int i, int drawn)
{
	char	buffer[MAX_FILE_LEN];
	int		c;

	c = 0xFFFFFF;
	ft_strlcpy(buffer, "  ", MAX_FILE_LEN);
	if (i == engine->bios.cursor_index)
	{
		c = 0xFFFF00;
		ft_strlcpy(buffer, "> ", MAX_FILE_LEN);
	}
	ft_strlcat(buffer, name, MAX_FILE_LEN);
	mlx_string_put(engine->mlx.mlx_ptr, engine->mlx.win_ptr, 100,
		150 + (drawn * 30), c, buffer);
}

/**
 * @fn void render_file_list(t_engine *engine, t_list *list)
 * @brief Iterates over an active directory list and renders the UI browser.
 * @details Incorporates a scrolling clamp. It visually renders a maximum of 
 * 12 items at once. If the cursor exceeds index 11, the starting point of the 
 * list shifts, effectively pushing the entire list upward to keep the cursor 
 * grounded aat the bottom of the visible UI.
 * @param engine The global engine context.
 * @param list   The linked list containing the parsed directory filenames.
 */
void	render_file_list(t_engine *engine, t_list *list)
{
	int	start_index;
	int	drawn;
	int	i;

	if (!list)
	{
		mlx_string_put(engine->mlx.mlx_ptr, engine->mlx.win_ptr, 100, 150,
			0xFF0000, "Directory empty or missing!");
		return ;
	}
	i = 0;
	drawn = 0;
	start_index = 0;
	if (engine->bios.cursor_index >= 12)
		start_index = engine->bios.cursor_index - 11;
	while (list && drawn < 12)
	{
		if (i >= start_index)
		{
			print_file_item(engine, (char *)list->content, i, drawn);
			drawn++;
		}
		list = list->next;
		i++;
	}
}
