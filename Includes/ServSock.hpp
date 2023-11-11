#ifndef SERVSOCK_HPP
#define SERVSOCK_HPP

#include "Socket.hpp"
#include "Server.hpp"
#include <algorithm>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include "../cgi/cgi_handler.hpp"

class ServSock
{
private:
    std::vector<std::pair<Socket, Server> > servSock;
    int i;
    std::string file;
    bool chunked;
    std::string chunkedData;
public:
    ServSock(/* args */);
    ~ServSock();
    int size();
    void remove(int);
    void addPair(std::pair<Socket, Server>);
    std::pair<Socket, Server>& operator[](int);
    void processConnection(int);
    void buildResponse(int, Response&, Request);
    void handleErrors(int, Response&, Request);
    std::string fct(std::string file, int& s);
    void buildHead(int n, Response&, Request);
    void sendResponse(int, Response);
    std::map<std::string, std::string> parseRequestHeader(Request rqt, int n, std::string cgi_path);
};




#endif