#ifndef SERVER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>

class Server
{
private:
    std::string host;
    std::vector<int> port;
    std::string server_name;
    std::string root;
    // std::map<std::string, std::string> root_index;
    

public:
    Server(/* args */);
    ~Server();
};

Server::Server(/* args */)
{
}

Server::~Server()
{
}


#endif