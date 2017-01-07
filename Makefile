##
## Makefile for L2GAME in /home/pjulien/LICENCE-INFORMATIQUE/LICENCE2/L2GAME
## 
## Made by Pignolet Julien
## Email   <julien.pignolet8@etu.univ-lorraine.fr>
## 
## Started on  Fri Sep 23 18:25:50 2016 Pignolet Julien
## Last update Sat Jan  7 14:11:11 2017 WillaimeNoah
##

NAME	= archi

SRC	= miniCPU.c \

OBJ	= $(SRC:.c=.o)

LDFLAGS	= -lSDL -lSDL_mixer -Wall -Wextra -Werror

CC	= gcc -g

RM	= rm -rf

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
