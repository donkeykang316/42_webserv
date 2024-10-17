NAME = webserver
CFLAGS = -Wall -Wextra -Werror
RM = rm -rf
FILES	=	main WebServer HTTPRequest HTTPResponse
SRC		= $(addsuffix .cpp, $(FILES))
OBJ		= $(addsuffix .o, $(FILES))

%.o : %.c
		c++ -std=c++98 -c $(CFLAGS) $< -o $@

all :	$(NAME)

$(NAME): $(OBJ)
	c++ -o $(NAME) $(OBJ)

$(OBJ) : $(SRC)
	c++ -std=c++98 $(CFLAGS) -c $(SRC)

clean:
	$(RM) $(OBJ)

fclean:	clean
	$(RM) $(NAME)

re:	fclean all

.PHONY: all	clean fclean re
