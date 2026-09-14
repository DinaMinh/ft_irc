NAME := ircserv

CXX := c++

INCLUDES := ./includes/

CXXFLAGS := -Wall -Wextra -Werror -I$(INCLUDES) -MMD -std=c++98

SRCS := main.cpp ParametersException.class.cpp ASocket.class.cpp Server.class.cpp \
		Client.class.cpp Channel.class.cpp

SRC_DIR := ./srcs/

OBJ_DIR := ./objects/
OBJ := $(addprefix $(OBJ_DIR), $(SRCS:.cpp=.o))
DEPS := $(OBJ:.o=.d)

BIN_DIR := ./bin/

YELLOW := \e[1;93m

GREEN := \e[1;4;92m

RED := \e[1;4;91m

NORMAL := \e[0m

ITALIC := \e[3m

DEBUG_FLAGS := $(CXXFLAGS) -g3

MAKEFLAGS += --no-print-directory -j -s

vpath %.cpp $(SRC_DIR)

all: $(BIN_DIR)$(NAME)

$(OBJ_DIR)%.o: %.cpp | $(OBJ_DIR)
	echo -e "[$(YELLOW)COMPILING$(NORMAL)] $@"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	echo -e "[$(ITALIC)CREATING OBJECT DIRECTORY$(NORMAL)]"
	mkdir $@

$(BIN_DIR):
	echo -e "[$(ITALIC)CREATING BIN DIRECTORY$(NORMAL)]"
	mkdir $@

$(BIN_DIR)$(NAME): $(OBJ) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@ 
	echo -----------------------------------------
	echo -e "|	Compiled $(GREEN)$(NAME)$(NORMAL) binary 💅	|"
	echo -----------------------------------------

debug: | $(BIN_DIR)
	$(CXX) $(DEBUG_FLAGS) -o $(BIN_DIR)$(NAME) $(addprefix $(SRC_DIR), $(SRCS))

clean:
	rm -rf $(OBJ_DIR)
	echo -------------------------------------------------
	echo -e "|	Removed $(RED)$(OBJ_DIR)$(NORMAL) directory	🗑️	|"
	echo -------------------------------------------------

fclean:
	$(MAKE) clean
	rm -rf $(BIN_DIR)
	echo -------------------------------------------------
	echo -e "|	Removed $(RED)$(BIN_DIR)$(NORMAL) directory	🗑️	|"
	echo -------------------------------------------------

re:
	$(MAKE) fclean
	$(MAKE) all

.PHONY: all clean fclean re

-include $(DEPS)
