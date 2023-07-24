#ifndef CONFIG_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <stack>

class Config
{
private:
    std::string config_file;
    std::string config;
public:
    Config(std::string file);
    ~Config();
    int parse_config();
    int check_brackets();
};

int Config::check_brackets(){
    std::stack<char> br;
    std::ifstream f(this->config_file);
    if (!f.is_open())
        return -1;
    std::string buf ;
    while (std::getline(f, buf)){
        if (buf.find("{", 0) != std::string::npos)
            br.push('{');
        else if (buf.find("}", 0) != std::string::npos)
            br.pop();
        std::cout <<"." << buf << std::endl;
    }
    if (!br.empty())
        return -1;
    
    return 0;
}

int Config::parse_config()
{
    if (check_brackets() < 0)
        return -1;
    std::ifstream file(this->config_file);
    if (!file.is_open())
        return -1;
    std::string buf ;
    while (std::getline(file, buf)){
        if 
    }
}

Config::Config(std::string file):config_file(file)
{
}

Config::~Config()
{
}


#endif