SRC = main.cpp cgi/cgi_handler.cpp srcs/Webserv.cpp srcs/Request.cpp srcs/Response.cpp \
		srcs/Server.cpp srcs/ServLocation.cpp srcs/ServSock.cpp srcs/Socket.cpp srcs/Config.cpp

FLAGS = -Wall -Wextra -Werror #-std=c++98

NAME = webserv

all: $(NAME)

$(NAME) : $(SRC)
	@c++ $(FLAGS) $(SRC) -o $(NAME)

clean : 
	@rm -rf $(NAME)

re: clean all

