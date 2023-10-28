#ifndef CONFIG_HPP

#include <string.h>
#include <fstream>
#include <iostream>
#include <stack>
#include <fcntl.h>
#include <unistd.h> 
#include <vector>
#include <iterator>
#include "ServSock.hpp"

class Config
{
private:
    std::string config_file;
    std::vector<std::string> configVec;
    std::vector<Server> servers;
public:
    Config();
    ~Config();
    int parse_config();
    int check_brackets(char *s);
    void printConfig();
    int confParse();
    void setConfigFileName(std::string s);
    int parse_server(std::vector<std::string>::iterator &b, std::vector<std::string>::iterator &e);
    std::vector<Server>* getServers();
};

std::string* splitIt(std::string s, int & size);

#endif