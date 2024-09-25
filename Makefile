NAME = Webserv

PATH = /srcs/

SRC = $(PATH)main.cpp\

OBJ	= $(SRC:.cpp=.o)

RM		= rm -f
CC		= c++
CFLAGS	= -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -O0 $(OBJ) -o $(NAME)

%.o:%.cpp
	$(CC) $(CFLAGS) -O0 -c $< -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re