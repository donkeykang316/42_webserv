SERVER = server

CLIENT = client

SRCDIR = src/
OBJDIR = obj/

CSRC = $(SRCDIR)Client.cpp\
		$(SRCDIR)cMain.cpp
SSRC = $(SRCDIR)Server.cpp\
		$(SRCDIR)sMain.cpp

COBJ = $(CSRC:$(SRCDIR)%.cpp=$(OBJDIR)%.o)
SOBJ = $(SSRC:$(SRCDIR)%.cpp=$(OBJDIR)%.o)

RM		= rm -fr
CC		= c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -O0

all: $(SERVER) $(CLIENT)

$(SERVER): $(SOBJ)
	$(CC) $(CFLAGS) $(SOBJ) -o $(SERVER)

$(CLIENT): $(COBJ)
	$(CC) $(CFLAGS) $(COBJ) -o $(CLIENT)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJDIR)*.o $(OBJDIR)

fclean: clean
	$(RM) $(SERVER) $(CLIENT)


re: fclean all

.PHONY: all clean fclean re