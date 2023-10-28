#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include "Socket.hpp"
#include "Config.hpp"

class Webserv{
private:
    std::vector<Socket> soc;
public:
    Config config;
    Webserv(std::string s);
    ~Webserv();
    int startParsing();
    int startSockets();
    int checkSocExistance(int);
};