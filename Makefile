CC=c++
NAME = ircserv
SRCS = main.cpp Channel.cpp Client.cpp Command.cpp Server.cpp
HEADER = Channel.hpp Client.hpp Command.hpp Server.hpp define.hpp
OBJS = $(SRCS:.cpp=.o)
CFLAGS = -Wall -Wextra -Werror -g -std=c++98

all:$(NAME)

$(NAME): $(OBJS)
	@printf "\033[1;37m"
	$(CC) $(OBJS) -o $(NAME)
	@printf "\033[1;32mCompilation Finished !\n\033[0m"

%.o : %.cpp $(HEADER)
	@printf "\033[1;37m"
	$(CC) $(CFLAGS) -I./$(INCPATH)  $< -c -o $@

fclean: clean
	@printf "\033[1;37m"
	rm -f $(NAME)
	@printf "\033[1;31mProgram Deleted !\n\033[0m"

clean:
	@printf "\033[1;37m"
	rm -f $(OBJS)
	@printf "\033[1;31mObjects Deleted !\n\033[0m"

re: fclean all

.PHONY: all fclean clean re
