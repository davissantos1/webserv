# Variables

CC= c++
CFLAGS= -Wall -Wextra -Werror -std=c++98 -Iinclude

SRC=	src/webserv.cpp \
		src/Session.cpp \
		src/Location.cpp \
		src/Multiplexer.cpp \
		src/ConfigParser.cpp \
		src/CgiHandler.cpp \
		src/CgiEnvironment.cpp \
		src/CgiParser.cpp \
		src/HttpRequest.cpp \
		src/HttpResponse.cpp \
		src/HttpResponseBuilder.cpp \
		src/string_utils.cpp \
		src/VirtualHostConfig.cpp \
		src/StaticFileHandler.cpp \
		src/Client.cpp \
		src/Server.cpp \
		src/HttpRequestParser.cpp \

SRC_TEST=

OBJ= $(SRC:.cpp=.o)
OBJ_TEST= $(SRC_TEST:.cpp=.o)

NAME= webserv

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

test_tokenizer:
	$(CC) $(CFLAGS) -g3 -Iinclude tests/tokenizer/tokenizer_runner.cpp src/ConfigParser.cpp src/Location.cpp src/VirtualHostConfig.cpp -o tests/tokenizer/runner_path.out && mv tests/tokenizer/runner_path.out .

test_config_parser:
	$(CC) $(CFLAGS) -g3 -Iinclude tests/parser/parser.cpp src/ConfigParser.cpp src/Location.cpp src/VirtualHostConfig.cpp -o tests/parser/parser.out && mv tests/parser/parser.out .

test_request_parser:
	$(CC) $(CFLAGS) -g3 -Iinclude tests/http_request/main.cpp src/HttpRequest.cpp src/HttpRequestParser.cpp src/string_utils.cpp -o tests/http_request/test_parser.out && tests/http_request/test_parser.out

.PHONY: all clean fclean re debug
