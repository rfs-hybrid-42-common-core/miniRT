/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine_bios_init.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 20:42:26 by maaugust          #+#    #+#             */
/*   Updated: 2026/07/12 21:48:57 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
#include "engine.h"
#include "libft.h"

/* --------------------------- Internal Libraries --------------------------- */
#include <dirent.h>
#include <stdlib.h>

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/**
 * @fn static void insert_sorted(t_list **list, t_list *new_node)
 * @brief Inserts a node into a linked list alphabetically.
 * @details Traverses the list and uses `ft_strncmp` to inject the new node 
 * in alphabetical order. Guarantees that the BIOS scene selector is neatly 
 * organized.
 * @param list     Double pointer to the root of the linked list.
 * @param new_node The incoming file node.
 */
static void	insert_sorted(t_list **list, t_list *new_node)
{
	t_list	*current;

	if (!*list || ft_strncmp((char *)(*list)->content,
		(char *)new_node->content, MAX_FILE_LEN) > 0)
	{
		new_node->next = *list;
		*list = new_node;
		return ;
	}
	current = *list;
	while (current->next && ft_strncmp((char *)current->next->content,
			(char *)new_node->content, MAX_FILE_LEN) <= 0)
		current = current->next;
	new_node->next = current->next;
	current->next = new_node;
}

/**
 * @fn static void add_scene_node(t_engine *engine, t_list **list, 
 * char *filename, DIR *dir)
 * @brief Allocates and appends a found filename to the directory list.
 * @details Duplicates the string and secures it in a new list node. If a 
 * malloc failure occurs during string duplication, it cleanly shuts down 
 * the open directory pointer before aborting the engine.
 * @param engine   Pointer to the global engine context.
 * @param list     Double pointer to the target linked list.
 * @param filename The exact string filename extracted from the directory.
 * @param dir      The active directory pointer (for safe closing on failure).
 */
static void	add_scene_node(t_engine *engine, t_list **list, char *filename,
	DIR *dir)
{
	char	*name;
	t_list	*node;

	name = ft_strdup(filename);
	if (!name)
	{
		closedir(dir);
		engine->exit_code = MALLOC;
		print_error("Malloc failed for BIOS scene string", NULL);
		exit_program(engine);
	}
	node = ft_lstnew(name);
	if (!node)
	{
		free(name);
		closedir(dir);
		engine->exit_code = MALLOC;
		print_error("Malloc failed for BIOS scene node", NULL);
		exit_program(engine);
	}
	insert_sorted(list, node);
}

/**
 * @fn static void load_dir_files(t_engine *engine, t_list **list, 
 * const char *path)
 * @brief Scans a system directory for files to display in the BIOS.
 * @details Opens the target directory and iterates through its contents, 
 * allocating and appending a node for every standard file found. Intentionally 
 * ignores hidden system files and directories (such as "." and "..") to ensure 
 * only visible files populate the user interface.
 * @param engine Pointer to the global engine context.
 * @param list   Double pointer to the list destined to hold the results.
 * @param path   The relative folder path to scan (e.g., "scenes/working/").
 */
static void	load_dir_files(t_engine *engine, t_list **list, const char *path)
{
	DIR				*dir;
	struct dirent	*entry;

	dir = opendir(path);
	if (!dir)
		return ;
	entry = readdir(dir);
	while (entry)
	{
		if (entry->d_name[0] != '.')
			add_scene_node(engine, list, entry->d_name, dir);
		entry = readdir(dir);
	}
	closedir(dir);
}

/**
 * @fn void init_bios(t_engine *engine)
 * @brief Pre-loads the BIOS state and directory lists.
 * @details Initializes the cursor limits for the Main Menu, and pre-fetches 
 * all available scenes so the user doesn't experience hanging lag when 
 * navigating.
 * @param engine Pointer to the global engine context.
 */
void	init_bios(t_engine *engine)
{
	char	*work_path;
	char	*broken_path;

	engine->bios.current_page = PAGE_MAIN_MENU;
	engine->bios.cursor_index = 0;
	engine->bios.max_items = PAGE_MAIN_MENU_ITEMS;
	work_path = ft_strjoin(engine->exec_dir, "scenes/working/");
	broken_path = ft_strjoin(engine->exec_dir, "scenes/broken/");
	load_dir_files(engine, &engine->bios.working_scenes, work_path);
	load_dir_files(engine, &engine->bios.broken_scenes, broken_path);
	free(work_path);
	free(broken_path);
}
