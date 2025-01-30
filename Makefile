NAME=webserv
CC=c++
FLAGS=-Wall -Werror -Wextra -std=c++98 -g
DIR_SRCS=srcs
DIR_OBJS=objects
DIR_INCS=includes

LST_SRCS=main.cpp Server.cpp
LST_OBJS=$(LST_SRCS:.cpp=.o)
LST_INCS=

SRCS=$(addprefix $(DIR_SRCS)/, $(LST_SRCS))
OBJS=$(addprefix $(DIR_OBJS)/, $(LST_OBJS))
INCS=$(addprefix $(DIR_INCS)/, $(LST_INCS))

$(DIR_OBJS)/%.o: $(DIR_SRCS)/%.cpp
	@mkdir -p $(DIR_OBJS)
	@$(CC) $(FLAGS) -I $(DIR_INCS) -c $< -o $@
	@echo -n '.'

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(OBJS) $(FLAGS) -o $@
	@echo "\n\nThe Woven Webserver compiled"

clean:
	@rm -f $(OBJS)
	@rm -rf objects
	@echo "All objects clean"

fclean: clean
	@rm -f $(NAME)
	@echo "And the executable too"

re: fclean all

.PHONY: all fclean clean re
