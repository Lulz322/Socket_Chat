NAME := server_app
NPWD := $(CURDIR)/$(NAME)

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	ECHO := echo -e
endif
ifeq ($(UNAME_S),Darwin)
	ECHO := echo

endif


CC := g++
CFLAGS := -Wall -Wextra -Werror -g3
IFLAGS := -I $(CURDIR)/includes/

OBJ_DIR = $(CURDIR)/objects

SRC := $(abspath $(wildcard srcs/*.cpp))


OBJ := $(SRC:.cpp=.o)

WHITE := \033[0m
BGREEN := \033[42m
GREEN := \033[32m
RED := \033[31m
YELLOW := \033[33m
INVERT := \033[7m

SUCCESS = [$(GREEN)✓$(WHITE)]

DEL := rm -rf

all: $(NAME)

$(OBJ): %.o: %.cpp
	@$(ECHO) -n ' $@: '
	@$(CC) $(CFLAGS) -c $(IFLAGS) $< -o $@
	@$(ECHO) "$(SUCCESS)"

$(NAME): $(OBJ)
	@$(ECHO) -n ' $(NPWD): '
	@$(CC) -pthread $(OBJ) $(LIBS) -o $(NAME)
	@$(ECHO) "[$(INVERT)$(GREEN)✓$(WHITE)]"

del:
	@$(DEL) $(OBJ)

clean:
	@$(DEL) $(OBJ)
	@$(ECHO) "$(YELLOW)objects deleted$(WHITE)"

fclean: clean
	@$(DEL) $(NAME)
	@$(ECHO) "$(INVERT)$(RED)deleted$(WHITE)$(INVERT): $(NPWD)$(WHITE)"

re: fclean all

.PHONY: all fclean clean re
