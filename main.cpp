#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Socket.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"

int main(int ac, char **av)
{
    Socket  soc;
    if (ac != 2)
    {
        std::cout << "Error:\n./webserv conf-file."<< std::endl;
        return 0;
    }
    Config config(av[1]);
    // if (soc.create_socket() < 0)
    //     return 0;
    // std::cout << "-> " << soc.get_request() << "." << std::endl;
    // Request request;
    // request.parse(soc.get_request());
    // std::cout << "{" << request.get_method() << "}" << std::endl;
    // std::cout << "{" << request.get_file() << "}" << std::endl;

    // Response response;
    // // response.
    // response.set_status(200);
    if (config.parse_config() < 0)
    {
        std::cout << "Something is wrong in conf file" << std::endl;
        return 0;
    }
    // config.
    soc.close_sock();
}
