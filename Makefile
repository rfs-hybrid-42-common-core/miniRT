# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/04/22 20:45:51 by maaugust          #+#    #+#              #
#    Updated: 2026/07/16 03:59:51 by maaugust         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================ PROJECT FILE NAMES ============================ #
NAME           = miniRT

# ============================== COMPILER FLAGS ============================== #
CC             = cc
CFLAGS         = -Wall -Wextra -Werror -pthread -O3 -MMD -MP
INCLUDES       = -Iinc -Ilibft/inc -Iminilibx
RM             = rm -rf

# ================================== COLORS ================================== #
GREEN          := \033[32m
RED            := \033[31m
YELLOW         := \033[33m
CYAN           := \033[36m
RESET          := \033[0m
BOLD           := \033[1m

# ================================== LIBFT =================================== #
LIBFT_PATH     = ./libft
LIBFT_LIB      = $(LIBFT_PATH)/libft.a

# ================================= MINILIBX ================================= #
MLX_PATH       = minilibx
MLX_LIB        = $(MLX_PATH)/libmlx_Linux.a
MLX_URL        = https://github.com/42paris/minilibx-linux.git

# =============================== SOURCE FILES =============================== #
SRC_PATH       = ./src
SRC            = $(shell find $(SRC_PATH) -name '*.c')

# =============================== OBJECT FILES =============================== #
OBJ_PATH       = ./obj
OBJ            = $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC))

# ============================ COMPILATION RULES ============================= #
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	@printf "$(CYAN)Compiling:$(RESET) $(YELLOW)$<$(RESET)\n"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# =============================== BUILD TARGETS ============================== #
all: $(NAME)

bonus: all

$(NAME): $(LIBFT_LIB) $(MLX_LIB) $(OBJ)
	@printf "$(GREEN)✔ miniRT objects built successfully.$(RESET)\n"
	@$(CC) $(CFLAGS) $(OBJ) $(LIBFT_LIB) $(MLX_LIB) -lXext -lX11 -lm -lz -o $(NAME)
	@printf "$(GREEN)$(BOLD)✔ Build complete → $(NAME)$(RESET)\n"

$(LIBFT_LIB):
	@printf "$(CYAN)→ Building Libft...$(RESET)\n"
	@$(MAKE) -C $(LIBFT_PATH) >/dev/null \
	  || { printf "$(RED)✖ Libft build failed!$(RESET)\n"; exit 1; }
	@printf "$(GREEN)✔ Libft built.$(RESET)\n"

$(MLX_LIB):
	@if [ ! -d "$(MLX_PATH)" ]; then \
		printf "$(CYAN)→ Downloading MiniLibX...$(RESET)\n"; \
		git clone $(MLX_URL) $(MLX_PATH) > /dev/null 2>&1 \
		|| { printf "$(RED)✖ MiniLibX download failed!$(RESET)\n"; exit 1; }; \
		printf "$(GREEN)✔ MiniLibX downloaded successfully.$(RESET)\n"; \
	fi
	@printf "$(CYAN)→ Building MiniLibX...$(RESET)\n"
	@$(MAKE) -C $(MLX_PATH) >/dev/null 2>&1 \
	  || { printf "$(RED)✖ MiniLibX build failed!$(RESET)\n"; exit 1; }
	@printf "$(GREEN)✔ MiniLibX built.$(RESET)\n"

# =============================== CLEAN TARGETS ============================== #
clean:
	@$(RM) $(OBJ_PATH)
	@rmdir -p --ignore-fail-on-non-empty $(OBJ_PATH) 2>/dev/null || true
	@$(MAKE) -C $(LIBFT_PATH) clean >/dev/null
	@if [ -d "$(MLX_PATH)" ]; then $(MAKE) -C $(MLX_PATH) clean >/dev/null 2>&1; fi
	@printf "$(YELLOW)• Cleaned object files.$(RESET)\n"

fclean: clean
	@$(RM) $(NAME) $(MLX_PATH)
	@$(MAKE) -C $(LIBFT_PATH) fclean >/dev/null
	@printf "$(RED)• Full clean complete.$(RESET)\n"

# ================================ NORMINETTE ================================ #
norm:
	@echo "Checking Norminette (ignoring MLX)..."
	@find . -type f \( -name "*.c" -o -name "*.h" \) | grep -vE "minilibx" \
		| xargs norminette

# ============================== REBUILD TARGETS ============================= #
re: fclean all

.PHONY: all bonus clean fclean re norm

# =============================== DEPENDENCIES =============================== #
-include $(OBJ:.o=.d)
