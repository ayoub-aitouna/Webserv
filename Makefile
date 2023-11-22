OBJ_DIR			:= .compiled

SRC				:= $(wildcard *.cpp)  $(wildcard **/*.cpp) $(wildcard **/**/*.cpp) $(wildcard **/**/**/*.cpp)

OBJ				:= $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))

NAME = WebServ

CC = g++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 
Sanitize		:= -g -fsanitize=address

all : Start

$(OBJ_DIR)/%.o : %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(Sanitize) -c $< -o $@

$(NAME) : $(OBJ)
	$(CC) $(CPPFLAGS) $(Sanitize) $(OBJ) -o $(NAME) 

Start: $(NAME)
	clear && ./$(NAME) conf/serv.conf

clean :
	rm $(OBJ)

fclean :
	rm -rf $(OBJ_DIR) $(NAME)

re : fclean $(NAME)