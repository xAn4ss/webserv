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
    int setPort(std::string *s, int& size);
    void setServerName(std::string server_name);
    int setRoot(std::string *root, int& size);

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

int Server::setPort(std::string *s, int& size){
    if (size == 1)
    {
        std::cout << "error in port directive." << std::endl;
        return 1;
    }
    for (int i = 1; i < size; i++)
    {
        if (s[i].find_first_not_of("1234567890") != -1)
        {
            std::cout << "error in ports." << std::endl;
            return 1; 
        }
        if (std::find(_port.begin(), _port.end(), atoi(s[i].c_str())) != _port.end())
        {
            std::cout << "port " << s[i] << " already used" << std::endl;
            return 1;
        }else
            _port.push_back(atoi(s[i].c_str()));

    }
    return 0;
}
int Server::setRoot(std::string *s, int& size)
{
    if (size == 1)
    {
        std::cout << "root missing." << std::endl;
        return 1;
    }
    
    return 0;
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