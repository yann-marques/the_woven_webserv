NAME=webserv

CXX=c++
FLAGS=-Wall -Werror -Wextra -g -std=c++98

DIR_SRCS=./srcs
DIR_OBJS=./objects
DIR_INCS=./includes

DIR_EXE=execution
DIR_PRS=parsing

INCLUDE= -I ./includes/$(DIR_EXE) -I ./includes/$(DIR_PRS)

MAIN_LST= main.cpp
MAIN_SRC= $(DIR_SRCS)/$(MAIN_LST)
MAIN_OBJ= $(DIR_OBJS)/$(MAIN_LST:.cpp=.o)

EXE_LST = HTTPReqExceptions.cpp HTTPRequest.cpp VServ.cpp \
	VServExceptions.cpp WebServ.cpp WebServExceptions.cpp
SRC_EXE= $(addprefix $(DIR_SRCS)/$(DIR_EXE)/, $(EXE_LST))
OBJ_EXE= $(addprefix $(DIR_OBJS)/$(DIR_EXE)/, $(EXE_LST:.cpp=.o))

PRS_LST= AParser_checkers.cpp check_utils.cpp Rules_exceptions.cpp \
	AParser.cpp Config_checkers.cpp Config_setters.cpp Rules_getters.cpp \
	AParser_exceptions.cpp Config.cpp Rules_checkers.cpp Rules_setters.cpp \
	AParser_setters.cpp Config_exceptions.cpp Rules.cpp StrException.cpp 
SRC_PRS= $(addprefix $(DIR_SRCS)/$(DIR_PRS)/, $(PRS_LST))
OBJ_PRS= $(addprefix $(DIR_OBJS)/$(DIR_PRS)/, $(PRS_LST:.cpp=.o))

LST_SRCS= $(MAIN_SRC) $(SRC_PRS) $(SRC_EXE)
LST_OBJS= $(MAIN_OBJ) $(OBJ_EXE) $(OBJ_PRS)

$(DIR_OBJS)/%.o: $(DIR_SRCS)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(FLAGS) -c $< -o $@ $(INCLUDE)
	@echo -n '.'

all: $(NAME)

$(NAME): $(LST_OBJS)
	@$(CXX) $(LST_OBJS) $(FLAGS) $(INCLUDE) -o $@
	@echo "\n\nThe Woven Webserver compiled"

clean:
	@rm -rf $(DIR_OBJS)
	@echo "All objects clean"

fclean: clean
	@rm -f $(NAME)
	@echo "And the executable too"

re: fclean all

.PHONY: all fclean clean re
