#ifndef SERVER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
class Server
{
private:
    std::string _host;
    std::vector<int> _port;
    std::string _server_name;
    std::string _root;
    // std::map<std::string, std::string> root_index;


public:
    Server(/* args */);
    ~Server();
    int setHost(std::string *host, int &size);
    void setPort(int port);
    void setServerName(std::string server_name);
    void setRoot(std::string root);

    void printPorts();
    std::string gethost();
};

Server::Server(/* args */)
{
}

int Server::setHost(std::string* host, int &size){
    if (size > 2 || size == 1)
    {
        std::cout << "Something is wrong with host directive." << std::endl;
        return 1;
    }
    this->_host = host[1];
    return 0;
}

void Server::setPort(int port){
    
    if (std::find(_port.begin(), _port.end(), port) != _port.end())
    {
        std::cout << "port " << port << " already used" << std::endl;
        exit(0);
    }
    
    _port.push_back(port);
}

void Server::printPorts(){
    std::vector<int>::iterator it = _port.begin();
    while (it != _port.end())
    {
        std::cout << *it << std::endl; 
        ++it;
    }
}

std::string Server::gethost(){
    return _host;
}

Server::~Server()
{
}


#endif