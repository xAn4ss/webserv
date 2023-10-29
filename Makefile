SRC = main.cpp Webserv.cpp Request.cpp Response.cpp \
		Server.cpp ServLocation.cpp ServSock.cpp Socket.cpp Config.cpp

FLAGS = -Wall -Wextra -Werror -std=c++98

NAME = webserv

all: $(NAME)

$(NAME) : $(SRC)
	@c++ $(FLAGS) $(SRC) -o $(NAME)

clean : 
	@rm -rf $(NAME)

re: clean all

