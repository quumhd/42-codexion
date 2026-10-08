# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/23 14:42:36 by jdreissi          #+#    #+#              #
#    Updated: 2026/10/08 10:30:21 by jdreissi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = codexion

CC = cc

CFLAGS = -Wall -Wextra -Werror -pthread


SRCS_DIR = coders

HEADER = $(SRCS_DIR)/codexion.h

SRCS = $(SRCS_DIR)/codexion.c \
	   $(SRCS_DIR)/coder_actions.c \
	   $(SRCS_DIR)/helper.c \
	   $(SRCS_DIR)/initialize_threads.c \
	   $(SRCS_DIR)/monitoring_routine.c \
	   $(SRCS_DIR)/parsing.c \
	   $(SRCS_DIR)/select_order.c \
	   $(SRCS_DIR)/manage_threads.c \

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
