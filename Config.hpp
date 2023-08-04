#ifndef CONFIG_HPP

#include <string.h>
#include <fstream>
#include <iostream>
#include <stack>
#include <fcntl.h>
#include <unistd.h> 
#include <vector>
#include <iterator>
#include "Server.hpp"

class Config
{
private:
    std::string config_file;
    std::vector<std::string> configVec;
    std::vector<Server> servers;
public:
    Config(std::string file);
    ~Config();
    int parse_config();
    int check_brackets(char *s);
    void printConfig();
    int confParse();
    int parse_server(std::vector<std::string>::iterator &b, std::vector<std::string>::iterator &e);
};

int Config::check_brackets(char *s){
    std::string config = s;
    int i = 0;
    int counter = 0;
    while ((i = config.find_first_of("{}", i)) != -1)
    {
        if (config.at(i) == '{'){
            counter++;
        }else
            counter--;
        i++;
    }
    if (counter)
        return -1;
    return 0;
}

void Config::printConfig(){
    std::vector<std::string>::iterator it = configVec.begin();
    while (it != configVec.end())
    {
        std::cout << *it << std::endl;
        it++;
    }
}

int Config::confParse(){
    char s[1042];
    int fd;

    std::string buf ;
    if ((fd = open(this->config_file.c_str(), O_RDONLY)) < 0)
    {
        std::cout << "Can't open." << std::endl;
        return -1;
    }
    int rd = read(fd, s, 1042);
    if (rd > 0){
        s[rd] = '\0';
    }else{
        std::cout << "Can't read." << std::endl;
        return -1;
    }
    if (check_brackets(s) < 0)
        return -1;
    int i = 0;
    char *x = strtok(s, "\t\n;");
    while (x){
        configVec.push_back(x);
        x = strtok(NULL, "\t\n;");
    }
    int b;
    i = 0;
    std::vector<std::string>::iterator it = configVec.begin();
    while (i < configVec.size())
    {
        int c = 0;
        if (!strncmp(configVec[i].c_str(), "server", 6) && (!strncmp(configVec[i+1].c_str(), "{", 1) || configVec[i].back() == '{'))
        {
            if(configVec[i+1].back() == '{')
                i++;
            i++;
            b = i;
            while (configVec[i].back() != '}')
            {
                if (!strncmp(configVec[i].c_str(), "location", 8) && (!strncmp(configVec[i+2].c_str(), "{", 1) || configVec[i+1].back() == '{'))
                {
                    if(!strncmp(configVec[i+2].c_str(), "{", 1))
                        i++;
                    i++;
                    while (!strncmp(configVec[i].c_str(), "}", 1))
                    {
                        i++;
                    }
                }
                i++;
            }
            i++;
            c = i;
            std::vector<std::string>::iterator bgin = it +b;
            std::vector<std::string>::iterator end = it +c +1;
            if (parse_server(bgin, end))
                return -1;
        }
        i++;
    }
    return 0;
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

int Config::parse_server(std::vector<std::string>::iterator &b, std::vector<std::string>::iterator &e){
    std::vector<std::string> conf(b, e);
    Server serv;
    std::vector<std::string>::iterator i = conf.begin();
    int s;
    while (i != conf.end())
    {
        s = 0;
        if (!strncmp("host", (*i).c_str(), 4))
        {
            if (serv.setHost(splitIt(*i, s), s))
                return 1;
        }
        else if (!strncmp("port", (*i).c_str(), 4))
        {
            if (serv.setPort(splitIt(*i, s), s))
                return 1;
        }
        else if (!strncmp("root", (*i).c_str(), 4))
        {

            if (serv.setRoot(splitIt(*i, s), s))
                return 1;
        }
        else if (!strncmp("index", (*i).c_str(), 5))
        {
            if (serv.setIndex(splitIt(*i, s), s))
                return 1;
        }
        else if (!strncmp("server_name", (*i).c_str(), 11))
        {
            if (serv.setServerName(splitIt(*i, s),s))
                return 1;
        }
        else if (!strncmp("error_page", (*i).c_str(), 10))
        {
            if (serv.setError(splitIt(*i, s), s))
                return 1;
        }
        else if (!strncmp("autoindex", (*i).c_str(), 9)){
            if (serv.setAutoIndex(splitIt(*i, s), s))
            return 1;
        }
        else if (!strncmp((*i).c_str(), "location", 8) &&
         (!strncmp((*(i + 1)).c_str(), "{", 1) || ((*i).back() == '{')))
        {
            ServLocation location;
            std::vector<std::string>::iterator begin = i;
            if (!strncmp((*(i + 1)).c_str(), "{", 1))
                i++;
            while (strncmp((*i).c_str(), "}", 1))
                i++;
            i++;
            location.processLocation(begin, i);
        }
        std::cout << "-> " << *i << std::endl;
        i++;
    }
    return 0;
}
/*
host
port
root (must not be empty)
index
server_name (goes by default to localhost)
error
auto index
return
client_max_body_size
location{
    error
    index
    root
    autoindex
    return
    client_max_body_size
    methods
    cgi_path
    cgi_exec
    upload
}
*/



int Config::parse_config()
{
    if (confParse() < 0)
        return -1;
    
    // printConfig();
    return 0;
}

Config::Config(std::string file):config_file(file)
{
}

Config::~Config()
{
}


#endif