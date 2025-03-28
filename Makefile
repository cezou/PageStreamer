NAME = PageStreamer
CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SRC_DIR = srcs
OBJ_DIR = obj
INC_DIR = includes

SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/StreamManager.cpp

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(NAME) is ready."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
