# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: replace <replace@student.42heilbronn.de    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/23 14:42:36 by replace           #+#    #+#              #
#    Updated: 2026/07/23 15:05:50 by jdreissi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = codexion

CC = cc

CFLAGS = -Wall -Wextra -Werror -Iinclude -pthread

HEADER = codexion.h

SRCS_DIR = srcs

SRCS = $(SRCS_DIR)/codexion.c \
	   $(SRCS_DIR)/coder_actions.c \
	   $(SRCS_DIR)/helper.c \
	   $(SRCS_DIR)/initialize_threads.c \
	   $(SRCS_DIR)/monitoring_routine.c \
	   $(SRCS_DIR)/parsing.c \
	   $(SRCS_DIR)/select_order.c \

O_FILES = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(O_FILES)
	$(CC) $(CFLAGS) $(O_FILES) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(O_FILES)

fclean: clean
	rm -f $(NAME)

re: fclean all
