#ifndef SERVER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <map>
#include <algorithm>
#include <dirent.h>
#include <unistd.h>
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
    std::map<std::string, int>  _methods;
    // std::map<std::string, std::string> root_index;


public:
    Server(/* args */);
    ~Server();
    int setMethods(std::string *s, int& size);
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
    std::map<std::string, int> getMethods();
    bool getAutoIndex();
};

#endif