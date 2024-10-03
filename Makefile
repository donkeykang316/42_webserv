NAME = webserv

SRCDIR = src/
OBJDIR = obj/

SRC = $(SRCDIR)Server.cpp\
		$(SRCDIR)Conf.cpp\
		$(SRCDIR)main.cpp

OBJ = $(SRC:$(SRCDIR)%.cpp=$(OBJDIR)%.o)

RM		= rm -fr
CC		= c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -O0

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJDIR)*.o $(OBJDIR)

fclean: clean
	$(RM) $(NAME)


re: fclean all

.PHONY: all clean fclean re