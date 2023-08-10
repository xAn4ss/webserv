#ifndef SERVSOCK_HPP
#define SERVSOCK_HPP

#include "Socket.hpp"
#include "Server.hpp"

class ServSock
{
private:
    std::vector<std::pair<Socket, Server>> servSock;
public:
    ServSock(/* args */);
    ~ServSock();
    int size();
    void remove(int);
    void addPair(std::pair<Socket, Server>);
    std::pair<Socket, Server>& operator[](int);
    void processConnection(int);
};

ServSock::ServSock(/* args */)
{
}

void ServSock::processConnection(int n){
    Request rqst;
    rqst.parse(servSock[n].first.get_request());
    std::cout << "=> " << rqst.get_method() << std::endl;
    std::cout << "=> " << rqst.get_file() << std::endl;
    std::cout << "=> " << rqst.get_http_v() << std::endl;
    if (!rqst.get_method().compare("GET"))
    {
        if (!rqst.get_file().compare("/"))
        {
            if (servSock[n].second.getLocation("/") != nullptr 
                && servSock[n].second.getIndex().empty())
            {
                std::cout << (*servSock[n].second.getLocation("/")).getLocationIndex() << std::endl;
                
            }else
            {
                std::cout << servSock[n].second.getIndex() << std::endl; 
            }
        }
    }
}

void ServSock::remove(int i){
    servSock.erase(servSock.begin()+i);
}

int ServSock::size(){
    return servSock.size();
}

void ServSock::addPair(std::pair<Socket, Server> ss){
    servSock.push_back(ss);
}
std::pair<Socket, Server>& ServSock::operator[](int i){
    return servSock[i];
}
ServSock::~ServSock()
{
}



#endif