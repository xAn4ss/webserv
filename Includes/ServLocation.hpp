#ifndef SERVLOCATION_HPP
#define SERVLOCATION_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <map>
#include <algorithm>
#include <unistd.h>
#include <dirent.h>
#include <cstring>

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
    std::map<std::string, int>  _methods;
public:
    ServLocation(/* args */);
    ~ServLocation();
    int setLocationRoot(std::string* s, int& size);
    int setLocationIndex(std::string* s, int& size);
    int setLocationPath(std::string s);
    int setLocationMethods(std::string *method, int &size);
    int setLocationAutoIndex(std::string *s, int& size);
    int processLocation(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end);
    std::string getLocationRoot();
    std::string getLocationPath();
    std::map<std::string, int> getLocationMethods();
    std::string getLocationIndex();
    bool getLocationAutoIndex();
    // int parse
};





#endif