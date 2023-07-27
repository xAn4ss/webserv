#ifndef CONFIG_HPP

#include <string.h>
#include <fstream>
#include <iostream>
#include <stack>
#include <fcntl.h>
#include <unistd.h> 
#include <vector>
#include <iterator>
class Config
{
private:
    std::string config_file;
    std::vector<std::string> configVec;
public:
    Config(std::string file);
    ~Config();
    int parse_config();
    int check_brackets(char *s);
    void printConfig();
    int confParse();
    void parse_server(std::vector<std::string>::iterator &b, std::vector<std::string>::iterator &e);
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
    char *x = strtok(s, " \t\n");
    while (x){
        configVec.push_back(x);
        x = strtok(NULL, " \t\n");
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
            std::cout << "==> " << *end << std::endl;
            parse_server(bgin, end);
        }
        i++;
    }
    return 0;
}

void Config::parse_server(std::vector<std::string>::iterator &b, std::vector<std::string>::iterator &e){
    std::vector<std::string> conf(b, e);
    std::vector<std::string>::iterator i = conf.begin();
    while (i != conf.end())
    {
        if (!strncmp())
        std::cout << "*" << *i << std::endl;
        i++;
    }
}

int Config::parse_config()
{
    if (confParse() < 0)
        return -1;
    
    printConfig();
    return 0;
}

Config::Config(std::string file):config_file(file)
{
}

Config::~Config()
{
}


#endif