#ifndef SERVER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <map>
#include <algorithm>
#include <dirent.h>
#include "ServLocation.hpp"

class Server
{
private:
    std::vector<ServLocation>  _location;
    bool                        _isAutoIndex;
    std::string                 _host;
    std::string                 _root;
    std::string                 _index;
    std::vector<int>            _port;
    std::vector<int>            _error_code;
    std::string                 _error_path;
    std::vector<std::string>    _server_name;
    // std::map<std::string, std::string> root_index;


public:
    Server(/* args */);
    ~Server();
    int setPort(std::string *s, int& size);
    int setHost(std::string *host, int &size);
    int setRoot(std::string *root, int& size);
    int setServerName(std::string *s, int& size);
    int setError(std::string *s, int& size);
    int setIndex(std::string *s, int& size);
    int setAutoIndex(std::string *s, int& size);
    void addLocation(ServLocation loc);
    void printPorts();
    std::vector<ServLocation>* getLocations();
    ServLocation* getLocation(std::string s);

    std::string gethost();
    int checkServ();
    std::vector<int> getPorts();
    std::string getIndex();
    std::string getRoot();
    std::string getErrorPath();
    bool getAutoIndex();
};

bool Server::getAutoIndex(){
    return _isAutoIndex;
}

std::string Server::getErrorPath(){
    if (_error_path.back() == '/')
        return _error_path;
    else
        return _error_path + "/";
}


std::string Server::getIndex(){
    return _index;
}

std::string Server::getRoot(){
    return _root;
}

ServLocation* Server::getLocation(std::string s){
            // std::cout << s << "." << std::endl;
    if (_location.empty())
        return nullptr;
    for (std::vector<ServLocation>::iterator it = _location.begin(); it != _location.end(); it++){
        if (!(*it).getLocationPath().compare(s))
        {
            return &(*it);
        }
    }
    return nullptr;
}

std::vector<int> Server::getPorts()
{
    return _port;
}

int Server::checkServ(){
    if (_port.empty())
    {
        std::cout << "No ports specidied." << std::endl;
        return -1;
    }
    if (_host.empty())
    {
        _host = "localhost";
        return -1;
    }
    int f = 0;
    for (int i = 0; i < _location.size(); i++)
    {
        std::cout << ".." << _location[i].getLocationPath() << std::endl;
        if (!strncmp("/" ,_location[i].getLocationPath().c_str() , 1))
        {
            std::cout << "=> " << f << std::endl;
            f = 1;
        }
        std::cout << "=> " << _location[i].getLocationAutoIndex() << std::endl;
        std::cout << "=> " << _location[i].getLocationIndex().empty() << std::endl;
        if (_location[i].getLocationAutoIndex() == true && _location[i].getLocationRoot().empty() == true)
        {
            std::cout << " AutoIndex On and index not empty. "<< std::endl;
            return -1;
        }
    }
    if (!f && _index.empty())
    {
        std::cout << "server doesn' t have root location" << std::endl;
        return -1;
    }
    return 0;
}

std::vector<ServLocation>* Server::getLocations(){

    if (_location.empty())
        return nullptr;
    return &_location;
}

Server::Server(/* args */)
{
    _isAutoIndex = false;
    _index = "";
}

void Server::addLocation(ServLocation loc){
    this->_location.push_back(loc);
}


int Server::setError(std::string *s, int& size){
    if (size == 1)
    {
        std::cout << "error in error_page" << std::endl;
        return 1;
    }
    int i = 1;
    while (i < size - 1)
    {
        if(s[i].find_first_not_of("0123456789") != -1)
        {
            std::cout << "error in error codes" << std::endl;
            return 1;
        }
        i++;
    }
    if (s[i].find_first_of("0123456789") != -1)
    {
        std::cout << "error" << std::endl;
        return 1;
    }else{
        if (!opendir(s[i].c_str()))
        {
            std::cout << "error pages path is wrong" << std::endl;
            return 1;
        }else
            _error_path = s[i];
    }

    return 0;
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

int Server::setIndex(std::string *s, int& size){
    if (size != 2)
    {
        std::cout << "missing index page" << std::endl;
        return 1;
    }
    std::string path = _root + s[1];
    if (access(path.c_str(), F_OK) == -1)
    {
        std::cout << "wrong server index path"<< std::endl;
        return 1;
    }
    _index = path;
    return 0;
}

int Server::setServerName(std::string *s, int& size){
    if (size < 2)
    {
        std::cout << "error in server_name directive." << std::endl;
        return 1;
    }
    for (int i = 1; i < size; i++){
        _server_name.push_back(s[i]);
    }
    return 0;
}

int Server::setRoot(std::string *s, int& size)
{
    if (size != 2)
    {
        std::cout << " error in root directive." << std::endl;
        return 1;
    }
    if (!opendir(s[1].c_str()))
    {
        std::cout << "error in root path." << std::endl;
        return 1;
    }
    _root = s[1];
    return 0;
}

int Server::setAutoIndex(std::string *s, int& size){
    if (size != 2)
    {
        std::cout << "error in auto index directive." << std::endl;
        return 1;
    }
    if (!strncmp("on", s[1].c_str(), 2))
        _isAutoIndex = true;
    else if (!strncmp("off", s[1].c_str(), 3))
        _isAutoIndex = false;
    
    return 0;
}

void Server::printPorts(){
    if (_port.empty())
    {
        std::cout << "(no ports specified)" << std::endl;
        return ;
    }
    std::vector<int>::iterator it = _port.begin();
    while (it != _port.end())
    {
        std::cout << *it << " ";
        ++it;
    }
    std::cout << std::endl;
}

std::string Server::gethost(){
    return _host;
}

Server::~Server()
{
}


#endif