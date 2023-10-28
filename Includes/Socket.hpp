#ifndef SOCKET_HPP
#define SOCKET_HPP
#include <unistd.h>
#include <iostream>
#include "Response.hpp"

class Socket
{
private:
    std::string request;
    int sock;
public:
    Socket();
    ~Socket();
    void set_request(std::string req);
    std::string get_request();
    int get_socket();
    void setSocket(int);
    int create_socket(int);
    void close_sock();
    void handleRequest();
};


#endif