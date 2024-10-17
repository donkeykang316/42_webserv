TOPDIR	:= $(shell pwd)/
SRCDIR	:= $(TOPDIR)src/
OBJDIR	:= $(TOPDIR)obj/
BINDIR	:= $(TOPDIR)
NAME	:= webserver
EXE		:= $(NAME)
OBJSUBDIR	:= $(TOPDIR)obj/Configuration

SFILES	:= cpp
OFILES	:= o
CC		:= c++
CFLAGS	:= -c -Wall -Wextra -Werror -std=c++98

SOURCES := $(shell find $(SRCDIR) -name "*.$(SFILES)")
OBJECTSSUBFOLDERS := $(addprefix $(OBJDIR)/, $(shell cd src && printf -- '%s\n' */ && cd ../))
OBJECTS := $(patsubst $(SRCDIR)%.$(SFILES), $(OBJDIR)%.$(OFILES), $(SOURCES))

ALLFILES := $(SOURCES)

.PHONY: all clean

all:    $(EXE)

$(OBJECTS): | $(OBJDIR) $(OBJECTSSUBFOLDERS)

$(OBJDIR):
	mkdir -p $(OBJDIR)
$(OBJECTSSUBFOLDERS):
	mkdir -p $(OBJECTSSUBFOLDERS)

$(EXE): $(OBJECTS)

		@$(CC) $^ -o $@

$(OBJDIR)%$(OFILES):    $(SRCDIR)%$(SFILES)
		@$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f $(OBJECTS) $(EXE)
	@rm -rf $(OBJECTSSUBFOLDERS)
	@rm -rf $(OBJDIR)

fclean:	clean
	$(RM) $(EXE)

re:	fclean all

.PHONY: all	clean fclean re
