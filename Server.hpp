#ifndef SERVER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <map>
#include <algorithm>
#include <dirent.h>


class ServLocation
{
private:
    std::string                 _locationPath;
    std::vector<int>            _error_code;
    std::string                 _error_path;
    std::string                 _index;
    std::string                 _root;
    bool                        _isAutoIndex;
    std::string                 _return;
    std::string                 _client_max_body_size;
public:
    ServLocation(/* args */);
    ~ServLocation();
    int setLocationRoot(std::string* s, int& size);
    int setLocationIndex(std::string* s, int& size);
    int processLocation(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end);
    std::string getLocationRoot();
    std::string getLocationPath();
    std::string getLocationIndex();
    // int parse
};
std::string ServLocation::getLocationIndex(){
    return _index;
}

std::string ServLocation::getLocationPath(){
    if (_locationPath.empty())
        return "empty"; 
    return _locationPath;
}
std::string ServLocation::getLocationRoot(){
    return _root;
}

std::string* splitIt(std::string s, int & size){
    int start = 0;
    int end = 0;
    std::vector<std::string> tmp;
    while (end != -1)
    {
        end = s.find_first_of(" \t", start);
        if (!s.substr(start, end - start).empty())
            tmp.push_back(s.substr(start, end - start));
        start = end + 1;
    }
    int x = -1;
    size = tmp.size();
    std::string *line = new std::string[size];
    while (++x < size)
        line[x] = tmp[x];
    return (line);
}

int ServLocation::setLocationIndex(std::string* s, int& size){
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
    _index = s[1];
    // maybe replace it with path
    return 0;
}

int ServLocation::setLocationRoot(std::string* s, int& size){
    if (size != 2)
    {
        std::cout << " error in Location \"" << _locationPath << "\" root directive." << std::endl;
        return 1;
    }
    if (!opendir(s[1].c_str()))
    {
        std::cout << "error in location root path." << std::endl;
        return 1;
    }
    _root = s[1];
    return 0;
}

int ServLocation::processLocation(std::vector<std::string>::iterator begin,std::vector<std::string>::iterator end){
    std::vector<std::string> lct(begin, end);
    // std::cout << "-* "<<(*begin).substr((*begin).find_first_of(" \t")+1,
    //  ((*begin).size()- (*begin).find_last_not_of("{ \t"))) << std::endl;

    int b = (*begin).find_first_of(" \t") + 1;
    int e = (*begin).find_last_not_of("{ \t") ;

    // std::cout << b <<" *-* " << e << " **" << (*begin).substr(b, e + 1 -b) <<".."<<std::endl;
    _locationPath = (*begin).substr(b, e + 1 -b);
    int size;
    // std::cout << "== " << _locationPath << std::endl;
    begin++;
    if ((*begin).back() == '{')
        begin++;
    while (begin != end)
    {
        size = 0;
        if (!strncmp((*begin).c_str(), "root", 4))
        {
            if (setLocationRoot(splitIt(*begin, size), size))
                return 1;
        }
        else if (!strncmp((*begin).c_str(), "index", 5))
        {
            if (setLocationIndex(splitIt((*begin).c_str(), size), size))
                return 1;
        }
        // std::cout << *begin << std::endl;
        begin++;
    }



    return 0;
}

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
    std::vector<ServLocation> getLocations();
    std::string gethost();
};

ServLocation::ServLocation(/* args */)
{
}

ServLocation::~ServLocation()
{
}

std::vector<ServLocation> Server::getLocations(){
    return _location;
}

Server::Server(/* args */)
{
    _isAutoIndex = false;
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
        }
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
    _index = s[1];
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