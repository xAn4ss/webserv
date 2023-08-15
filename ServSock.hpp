#ifndef SERVSOCK_HPP
#define SERVSOCK_HPP

#include "Socket.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include <fstream>
#include <algorithm>
#include <stdlib.h>

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
    Request     rqst;
    Response    rsp;
    // std::cout << "salam "<< std::endl;
    rqst.parse(servSock[n].first.get_request());
        std::cout << "=== " << rqst.get_method() << std::endl;
    if (!rqst.get_method().compare("GET"))
    {
        if (!rqst.get_file().compare("/"))
        {
            std::string path;
            if (servSock[n].second.getLocation("/") != nullptr 
                && servSock[n].second.getIndex().empty())
            {
                path = (*servSock[n].second.getLocation("/")).getLocationIndex();
            }else
                path = servSock[n].second.getIndex();
                // looks for index in location
            std::cout << path << std::endl;
            rsp.set_status(200);
            std::string tmp ;
            tmp += rqst.get_http_v() + " ";
            tmp += rqst.get_method() + " OK\n";
            tmp += "Content-Type: text/html\n\n";
            std::ifstream f(path);
            std::string tm;
            while (getline(f, tm)){
                tmp += tm + '\n';
            }
            std::cout << "==> "<< tmp << std::endl;
            if (send(servSock[n].first.get_socket(), tmp.c_str(), strlen(tmp.c_str()), 0) == -1){
                std::cout << "error" << std::endl;
            }
            servSock[n].first.close_sock();
            servSock.erase(servSock.begin() + n); 
            std::cout << "====" << std::endl;              
        }
        else
        {
                std::string file;
            if (servSock[n].second.getLocation("/") != nullptr 
                && servSock[n].second.getIndex().empty()){
                    // if (servSock[n].second.getLocation(rqst.get_file()) != nullptr)
                    // {
                    //     file = (*servSock[n].second.getLocation(rqst.get_file())).getLocationIndex();
                    //     std::cout << "--> " << file << std::endl;
                    // }
                    // else
                        file = (*servSock[n].second.getLocation("/")).getLocationRoot();
                }
            else if (servSock[n].second.getLocation("/") == nullptr 
                && !servSock[n].second.getIndex().empty())
                file = servSock[n].second.getRoot();
                
            file.erase(file.end()-1);
            file += rqst.get_file();
            rsp.set_status(200);
            std::string tmp ;
            tmp += rqst.get_http_v() + " ";
            tmp += std::to_string(rsp.get_status()) + " OK\n";
            tmp += "Content-Type: ";
            std::cout << file.length() - file.find(".css") << std::endl;
            if ((file.length() - file.find(".css")) == 4){
                tmp += "text/css\n\n";
            }else if((tmp.length() - tmp.find(".html")) == 5){
                tmp += "text/html\n\n";
            }
            // text/html\n\n";
            std::cout << "<========" << file << "=========> "<< std::endl;
            std::ifstream f(file);
            std::string tm;
            while (getline(f, tm)){
                tmp += tm + '\n';
            }
            std::cout << "==> "<< tmp << std::endl;
            if (send(servSock[n].first.get_socket(), tmp.c_str(), strlen(tmp.c_str()), 0) == -1){
            std::cout << "error" << std::endl;
            }
            std::cout << "====" << std::endl; 
            servSock[n].first.close_sock();
            servSock.erase(servSock.begin() + n);                 
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