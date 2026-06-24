# Variables

CC= c++
CFLAGS= -Wall -Wextra -Werror -std=c++98
SRC= 
SRC_TEST= 

OBJ= $(SRC:.cpp=.o)
OBJ_TEST= $(SRC_TEST:.cpp=.o)
NAME= webserv
NAME= webserv_test

# Makeflags
MAKEFLAGS += --no-print-directory

# Colors
RED := \033[31m
GREEN := \033[32m
YELLOW := \033[33m
BLUE := \033[34m
RESET := \033[0m

# Rules
all: $(NAME)

$(NAME): $(OBJ)
	@printf " 💻 ${GREEN}Building:${RESET} ${NAME}\n"
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	@printf " 🛠️ ${BLUE} Compiling:${RESET} $< to $@\n"
	@$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS += -g -o0
debug: re
	@printf " ⚠️  ${RED}Compilation mode:${RESET} debug\n"

test: SRC += SRC_TEST
test: re
	@printf " ${RED}Test mode:${RESET} compiling and running tests\n"
	./webserv_test

clean:
	@printf " 🧹 ${YELLOW}Cleaning: ${RESET}project objects\n"
	@rm -rf $(OBJ)

fclean: clean
	@printf " 💣 ${YELLOW}Cleaning: ${RESET}everything\n"
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re debug
