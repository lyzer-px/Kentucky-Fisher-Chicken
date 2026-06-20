##
## EPITECH PROJECT, 2025
## new_workspace_creator
## File description:
## Makefile
##

CXX		=	clang++

SRC     =	$(shell find src/ -wholename "*.cpp")

OBJ     =	$(SRC:.cpp=.o)

NAME    =	kfc

VPATH	=	include/

CXXFLAGS += -Wall -Wextra -std=c++20

CPPFLAGS = -iquote $(VPATH)

LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ) $(LDFLAGS)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)
	$(RM) *.gc*

re: fclean all

debug: CXXFLAGS += -g3 -DDEBUG
debug: re

tests_run: fclean
	$(MAKE) NAME=unit_tests \
		LDFLAGS="--coverage -lcriterion" \
		SRC="$(filter-out src/main.cpp,$(SRC)) $(shell find tests/src -name '*.cpp')" \
		all
	./unit_tests

coverage:
	gcovr --gcov-executable "llvm-cov gcov" --exclude tests/
	gcovr --gcov-executable "llvm-cov gcov" --exclude tests/ --txt-metric branch

.PHONY: all clean fclean re debug tests_run
