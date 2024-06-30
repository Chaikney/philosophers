# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: chaikney <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/20 11:36:56 by chaikney          #+#    #+#              #
#    Updated: 2023/05/17 15:40:23 by chaikney         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= philo

SRC		= philo.c utils.c setup.c actions.c monitor.c

OBJ		= $(SRC:.c=.o)

CFLAGS = -Werror -Wall -Wextra -pthread -Fthread-sanitize -ggdb
CC		= gcc

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean, all, fclean, re, libft

clean:
	/bin/rm -f *.o
	/bin/rm -f __.SYMDEF
	/bin/rm -rf *.dSYM

fclean: clean
	/bin/rm -f $(NAME)

re: fclean all
