#ifndef CONFIG_HPP

#include <string>
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
    std::string config;
public:
    Config(std::string file);
    ~Config();
    int parse_config();
    int check_brackets();
};

int Config::check_brackets(){
    std::stack<char> br;
    int i = 0;
    while ((i = config.find_first_of("{}", i)) != -1)
    {
        if (config.at(i) == '{'){
            br.push('{');
        }else
            br.pop();
        i++;
    }
    if (!br.empty())
        return -1;
    return 0;
}

int Config::parse_config()
{
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
    config = s;
    if (check_brackets() < 0)
        return -1;
    int i = 0;
    std::vector<std::string> configVec;
    char *x = strtok(s, " \t\n");
    configVec.push_back(x);
    while (x){
        // std::cout <<"->"<< config[i] <<""<< std::endl;
        // std::cout <<".."<< i << std::endl;
        std::cout <<"->"<< x << std::endl;
        configVec.push_back(x);
        x = strtok(NULL, " \t\n");
    }
    std::vector<std::string>::iterator it = configVec.begin();
    std::cout << "-> " << config << "." << std::endl;
    // " \t\n"
    
    return 0;
}

Config::Config(std::string file):config_file(file)
{
}

Config::~Config()
{
}


#endif