#ifndef SERVLOCATION_HPP
#define SERVLOCATION_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <map>
#include <algorithm>
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
    int setLocationAutoIndex(std::string *s, int& size);
    int processLocation(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end);
    std::string getLocationRoot();
    std::string getLocationPath();
    std::string getLocationIndex();
    bool getLocationAutoIndex();
    // int parse
};

ServLocation::ServLocation(){
    _index = "";
    _isAutoIndex = false;
    _methods.insert(std::make_pair("GET", 1));
    _methods.insert(std::make_pair("POST", 1));
    _methods.insert(std::make_pair("DELETE", 1));

}

ServLocation::~ServLocation()
{
}

bool ServLocation::getLocationAutoIndex()
{
    return _isAutoIndex;
}

std::string ServLocation::getLocationIndex(){
    if (this == nullptr)
        return "inexistant location.";
    
    if(_index.empty())
        return "";
    return _index;
}

std::string ServLocation::getLocationPath(){
    if (_locationPath.empty())
    {
        return "empty";
    }
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
    _index = path;
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

int ServLocation::setLocationAutoIndex(std::string *s, int& size){
    if (size != 2)
    {
        std::cout << "error in Location autoIndex directive." << std::endl;
        return 1;
    }
    if (!strncmp("on", s[1].c_str(), 2))
        _isAutoIndex = true;
    else if (!strncmp("off", s[1].c_str(), 3))
        _isAutoIndex = false;
    return 0;
}

int ServLocation::setLocationPath(std::string s)
{
    if (!opendir(s.c_str()))
    {
        std::cout << "error in location path" << std::endl;
        return 1;
    }
    _locationPath = s;
    return 0;
}
int ServLocation::processLocation(std::vector<std::string>::iterator begin,std::vector<std::string>::iterator end){
    std::vector<std::string> lct(begin, end);
    // std::cout << "-* "<<(*begin).substr((*begin).find_first_of(" \t")+1,
    //  ((*begin).size()- (*begin).find_last_not_of("{ \t"))) << std::endl;

    int b = (*begin).find_first_of(" \t") + 1;
    int e = (*begin).find_last_not_of("{ \t") ;

    // std::cout << b <<" *-* " << e << " **" << (*begin).substr(b, e + 1 -b) <<".."<<std::endl;
    if (setLocationPath((*begin).substr(b, e + 1 -b)))
        return 1;
    int size;
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
        }else if (!strncmp((*begin).c_str(), "autoIndex", 9))
        {
            if (setLocationAutoIndex(splitIt((*begin).c_str(), size), size))
                return 1;
        }
        // std::cout << *begin << std::endl;
        begin++;
    }



    return 0;
}




#endif