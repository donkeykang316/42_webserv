NAME	=  PmergeMe

RM		=	rm -rf

FLAGS	=	-Wall -Wextra -Werror -std=c++98 -g

SRC		=	PmergeMe main

SRCS	=	$(addsuffix .cpp, $(SRC))

OBJS	=	$(addsuffix .o, $(SRC))

%.o:%.c
	c++ -c $(FLAGS) $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	c++ -o $(NAME) $(OBJS)

$(OBJS): $(SRCS)
	c++ -c $(FLAGS) $(SRCS) -I ./

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

# JOT := 
ERROR_VAR := "fgd";
RANDOM_1 := $(shell echo `shuf -i 1-10000 -n 1 | tr '\n' ' '`)
RANDOM_2 := $(shell echo `shuf -i 1-10000 -n 2 | tr '\n' ' '`)
RANDOM_5 := $(shell echo `shuf -i 1-10000 -n 5 | tr '\n' ' '`)
RANDOM_7 := $(shell echo `shuf -i 1-10000 -n 7 | tr '\n' ' '`)
RANDOM_TEN := $(shell echo `shuf -i 1-10000 -n 10 | tr '\n' ' '`)
RANDOM_11 := $(shell echo `shuf -i 1-10000 -n 11 | tr '\n' ' '`)
RANDOM_100 := $(shell echo `shuf -i 1-10000 -n 100 | tr '\n' ' '`)
RANDOM_5000 := $(shell echo `shuf -i 1-10000 -n 5000 | tr '\n' ' '`)

run:
	- @echo "\033[92mERROR CASE \"./$(NAME)\"\033[0m"; ./$(NAME)
	@echo "\033[92m \"./$(NAME) 3 5 9 7 4 \"\033[0m"; ./$(NAME) 3 5 9 7 4
	@echo "\033[92m \"./$(NAME) 0 0 0 0 0 \"\033[0m"; ./$(NAME) 0 0 0 0 0
	@echo "\033[92m \"./$(NAME) 0 0 0 0 10 \"\033[0m"; ./$(NAME) 0 0 0 0 10
	@echo "\033[92m \"./$(NAME) 1 0 0 0 0 10 \"\033[0m"; ./$(NAME) 1 0 0 0 0 10
	@echo "\033[92m \"./$(NAME) 1 0 0 0 0  \"\033[0m"; ./$(NAME) 1 0 0 0 0
	@echo "\033[92m \"./$(NAME) 1 0 0 0 0 0 \"\033[0m"; ./$(NAME) 1 0 0 0 0 0
	@echo "\033[92m \"./$(NAME) 0 1 0 0 0 0 \"\033[0m"; ./$(NAME) 0 1 0 0 0 0
	@echo "\033[92m \"./$(NAME) 0 1 0 0 0 \"\033[0m"; ./$(NAME) 0 1 0 0 0
	@echo "\033[92m \"./$(NAME) 1 2 3 \"\033[0m"; ./$(NAME) 1 2 3
	@echo "\033[92m \"./$(NAME) 1 -2 3 \"\033[0m"; ./$(NAME) 1 -2 3


	@echo "\033[92mSORTING of 1 RANDOM\"./$(NAME) ${RANDOM_1} \"\033[0m"; ./$(NAME) ${RANDOM_1}
	@echo "\033[92mSORTING of 2 RANDOM\"./$(NAME) ${RANDOM_2} \"\033[0m"; ./$(NAME) ${RANDOM_2}
	@echo "\033[92mSORTING of 5 RANDOM\"./$(NAME) ${RANDOM_5} \"\033[0m"; ./$(NAME) ${RANDOM_5}
	@echo "\033[92mSORTING of 10 RANDOM\"./$(NAME) ${RANDOM_TEN} \"\033[0m"; ./$(NAME) ${RANDOM_TEN}
	@echo "\033[92mSOLUTION IS 42 \"./$(NAME) ${RANDOM_11} \"\033[0m"; ./$(NAME) ${RANDOM_11}
	@echo "\033[92mSOLUTION IS 42 \"./$(NAME) ${RANDOM_100} \"\033[0m"; ./$(NAME) ${RANDOM_100}
	# @echo "\033[92mSOLUTION IS 42 \"./$(NAME) ${RANDOM_5000} \"\033[0m"; ./$(NAME) ${RANDOM_5000}
	@echo "\033[92mSOLUTION IS 42 \"./$(NAME) ${ERROR_VAR} \"\033[0m"; ./$(NAME) ${ERROR_VAR}

	valgrind -s --leak-check=full --show-leak-kinds=all ./$(NAME)

.PHONY: re fclean clean all $(NAME)
