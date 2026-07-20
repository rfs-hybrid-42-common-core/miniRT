# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/03 18:21:29 by maaugust          #+#    #+#              #
#    Updated: 2026/06/22 13:57:42 by maaugust         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================ PROJECT FILE NAMES ============================ #
NAME             = libft.a

# ============================== COMPILER FLAGS ============================== #
CC               = cc
CFLAGS           = -Wall -Wextra -Werror -MMD -MP
INCLUDES         = -Iinc
AR               = ar rcs
RM               = rm -rf

# ================================== COLORS ================================== #
GREEN            := \033[32m
RED              := \033[31m
YELLOW           := \033[33m
CYAN             := \033[36m
RESET            := \033[0m
BOLD             := \033[1m

# =============================== SOURCE FILES =============================== #
SRC_PATH    = src
SRC         = $(shell find $(SRC_PATH) -name '*.c')

# =============================== OBJECT FILES =============================== #
OBJ_PATH    = obj
OBJ         = $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC))

# ============================ COMPILATION RULES ============================= #
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	@printf "$(CYAN)Compiling libft:$(RESET) $(YELLOW)$<$(RESET)\n"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# =============================== BUILD TARGETS ============================== #
all: $(NAME)

$(NAME): $(OBJ)
	@printf "$(GREEN)✔ Libft objects built successfully.$(RESET)\n"
	@$(AR) $(NAME) $(OBJ)
	@printf "$(GREEN)$(BOLD)✔ Library created → $(NAME)$(RESET)\n"

# ============================== CLEAN TARGETS =============================== #
clean:
	@$(RM) $(OBJ_PATH)
	@printf "$(YELLOW)• Cleaned libft object files.$(RESET)\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "$(RED)• Libft full clean complete.$(RESET)\n"

re: fclean all

# ============================== PHONY TARGETS =============================== #
.PHONY: all clean fclean re

# =============================== DEPENDENCIES =============================== #
-include $(OBJ:.o=.d)
