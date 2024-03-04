# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pbremond <pbremond@student.42nice.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/25 15:25:19 by pbremond          #+#    #+#              #
#    Updated: 2024/03/01 21:17:00 by pbremond         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Colors
_BLACK			= \033[0;30m
_RED 			= \033[0;31m
_GREEN			= \033[0;32m
_BROWN			= \033[0;33m
_BLUE			= \033[0;34m
_PURPLE			= \033[0;35m
_CYAN			= \033[0;36m
_LIGHT_GRAY		= \033[0;37m
_DARK_GRAY		= \033[1;30m
_LIGHT_RED		= \033[1;31m
_LIGHT_GREEN	= \033[1;32m
_YELLOW			= \033[1;33m
_LIGHT_BLUE		= \033[1;34m
_LIGHT_PURPLE	= \033[1;35m
_LIGHT_CYAN		= \033[1;36m
_WHITE			= \033[1;37m
_NC 			= \033[0m

# Inverted, i.e. colored backgrounds
_IGREY			= \x1b[40m
_IRED			= \x1b[41m
_IGREEN			= \x1b[42m
_IYELLOW		= \x1b[43m
_IBLUE			= \x1b[44m
_IPURPLE		= \x1b[45
_ICYAN			= \x1b[46m
_IWHITE			= \x1b[47m

# Color reset
_COLOR_RESET	= \033[0m

# Folders
INCLUDES = -I./include -I./libft/include
SRC_DIR = src
TEST_DIR = tester

# CC = gcc
CFLAGS := -Wall -Wextra -std=gnu17 -fno-builtin-malloc $(INCLUDES)

LIBFT := libft.a
LIBFT_PATH := libft

LDFLAGS := -L./$(LIBFT_PATH) -L.
LDLIBS := -lft -lpthread

LIBFT_BUILD_ARGS := CFLAGS=-std=gnu17 CC=$(CC)

ifdef BUILD_DEBUG
	OBJ_DIR := objs_debug
	TGT_DIR := debug
	CFLAGS += -gdwarf-4 -Og -fno-omit-frame-pointer
	LIBFT_BUILD_ARGS += BUILD_DEBUG=1
else
	OBJ_DIR := objs
	TGT_DIR := release
	CFLAGS += -O3 -Werror
	CPPFLAGS += -DNDEBUG
endif

ifndef VALGRIND
	CFLAGS += -fsanitize=address
	LDFLAGS += -fsanitize=address
endif

# ============================================================================ #
# ============================================================================ #

# Source files used in both mandatory and bonus parts
SRC_COMMON =	malloc.c calloc.c free.c realloc.c

# ============================================================================ #
# ============================================================================ #

# Source files exclusive to mandatory part
SRC_MANDATORY =	$(SRC_COMMON)

SRC_MANDATORY_PLUS_PATH = $(addprefix $(SRC_DIR)/, $(SRC_MANDATORY))
MANDATORY_OBJ = $(subst $(SRC_DIR)/, $(OBJ_DIR)/, $(patsubst %.c, %.o, $(SRC_MANDATORY_PLUS_PATH)))

# ============================================================================ #
# ============================================================================ #

# Source files exclusive to bonus part
SRC_BONUS =	$(SRC_COMMON)

SRC_BONUS_PLUS_PATH = $(addprefix $(SRC_DIR)/, $(SRC_BONUS))
BONUS_OBJ = $(subst $(SRC_DIR)/, $(OBJ_DIR)/, $(patsubst %.c, %.o, $(SRC_BONUS_PLUS_PATH)))

# ============================================================================ #
# ============================================================================ #

TEST_SRC =	test.c
TEST_SRC_PLUS_PATH = $(addprefix $(TEST_DIR)/, $(TEST_SRC))
TEST_OBJ = $(subst $(TEST_DIR)/, $(OBJ_DIR)/, $(patsubst %.c, %.o, $(TEST_SRC_PLUS_PATH)))

# ============================================================================ #
# ============================================================================ #

HOSTTYPE ?= $(shell uname -m)_$(shell uname -s)

TARGET = $(TGT_DIR)/libft_malloc_$(HOSTTYPE).so
TARGET_BONUS = $(TGT_DIR)/libft_malloc_$(HOSTTYPE)_bonus.so

TESTER_TARGET = $(TGT_DIR)/test_ft_malloc

all: $(TARGET) $(TESTER_TARGET)

bonus: $(TARGET_BONUS) $(TESTER_TARGET)

$(TARGET): $(LIBFT_PATH)/$(LIBFT) $(MANDATORY_OBJ)
	@echo "$(_PURPLE)Linking $(TARGET)$(_COLOR_RESET)"
	@mkdir -p $(@D)
	@$(CC) $(MANDATORY_OBJ) -shared -o $(TARGET) $(LDFLAGS) $(LDLIBS)
	@ln -sf $(TARGET) ./libft_malloc.so

$(TARGET_BONUS): $(LIBFT_PATH)/$(LIBFT) $(BONUS_OBJ)
	@echo "$(_PURPLE)Linking $(TARGET_BONUS)$(_COLOR_RESET)"
	@mkdir -p $(@D)
	@$(CC) $(BONUS_OBJ) -shared -o $(TARGET_BONUS) $(LDFLAGS) $(LDLIBS)
	@ln -sf $(TARGET_BONUS) ./libft_malloc.so

$(TESTER_TARGET): $(LIBFT_PATH)/$(LIBFT) $(TEST_OBJ)
	@echo "$(_PURPLE)Linking $(TESTER_TARGET)$(_COLOR_RESET)"
	@mkdir -p $(@D)
	@$(CC) $(TEST_OBJ) -o $(TESTER_TARGET) $(LDFLAGS) $(LDLIBS) -lft_malloc

$(LIBFT_PATH)/$(LIBFT):
	@echo "$(_PURPLE)Making $(basename $(LIBFT))$(_COLOR_RESET)"
	@make -C $(LIBFT_PATH) $(LIBFT_BUILD_ARGS)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@echo "$(_BLUE)Compiling $(basename $(notdir $*.o)) $(_COLOR_RESET)"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -fPIC -c $< -o $@

$(OBJ_DIR)/%.o : $(TEST_DIR)/%.c
	@echo "$(_BLUE)Compiling $(basename $(notdir $*.o)) $(_COLOR_RESET)"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

re: fclean all

fclean: clean
	@echo "$(_RED)Deleting binaries$(_COLOR_RESET)"
	@rm -rf $(TARGET) $(TARGET_BONUS) $(TESTER_TARGET)

clean:
	@echo "$(_RED)Cleaning object files$(_COLOR_RESET)"
	@rm -rf $(OBJ_DIR)

.PHONY: clean fclean re all bonus
