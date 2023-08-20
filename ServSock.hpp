#ifndef SERVSOCK_HPP
#define SERVSOCK_HPP

#include "Socket.hpp"
#include "Server.hpp"
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <sys/stat.h>
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
    rqst.parse(servSock[n].first.get_request());
        std::cout << "=== " << rqst.get_method() << std::endl;
        std::cout << "=== " << rqst.get_file() << std::endl;
    rsp.set_status(69);
    if (!rqst.get_method().compare("GET"))
    {
        // if dir => check index of that directory in server lcoation
        // else if file 
        //      => locationroot + file;
        //      => serverRoot + file;

            std::string file;
            if (rqst.get_file().compare("/"))
                rqst.setFile(rqst.get_file().substr(1, strlen(rqst.get_file().c_str())));
            struct stat slatt;
            // file = rqst.get_file();
            if (!stat(rqst.get_file().c_str(), &slatt))
            {
                std::cout << "-*-*-*--*-*-" << rqst.get_file() << "*-*-*-*--*-*--*-" << std::endl;
                if (S_ISDIR(slatt.st_mode))
                {
                    std::cout << "dir "<< std::endl;
                    if (servSock[n].second.getLocation(rqst.get_file()) == nullptr 
                        && !servSock[n].second.getIndex().empty()){
                            struct stat t;
                            if (!stat(rqst.get_file().c_str(), &t))
                                rsp.set_status(403);
                            else{
                                file = servSock[n].second.getIndex();
                                rsp.set_status(200);
                            }
                        std::cout << "... " << file << std::endl; 
                    }
                    if (servSock[n].second.getLocation(rqst.get_file()))
                    {    // when u give a folder path u get index to that location/folder
                        file = (*servSock[n].second.getLocation(rqst.get_file())).getLocationIndex();
                        rsp.set_status(200);
                    }else if (servSock[n].second.getLocation(rqst.get_file()+"/"))
                    {
                        file = rqst.get_file() + "/";
                        rsp.set_status(301);
                    }
                }else{
                    std::cout << "file" << std::endl;
                    std::string path;
                    if (!access(rqst.get_file().c_str(), F_OK))
                    { // if path requested exists file = path
                        if (servSock[n].second.getLocation(rqst.get_file().substr(0, rqst.get_file().find_last_of("/")))){
                            file = rqst.get_file();
                            rsp.set_status(200);
                        }else
                            rsp.set_status(403);
                        std::cout << "&&& " << file << std::endl; 
                    }
                }
            }else{
    // if path doesnt mathc any file file = root + file; (root from serv or location)
                std::string path = rqst.get_file();
                if (servSock[n].second.getLocation("/") != nullptr 
                    && servSock[n].second.getIndex().empty())
                    {// file = locationroot + file requested
                        path = (*servSock[n].second.getLocation("/") ).getLocationRoot() + rqst.get_file();
                        if (!access(path.c_str(), F_OK)){
                            file = path;
                            rsp.set_status(200);
                        }else
                            rsp.set_status(404);
                        std::cout <<">>>> "<< file<< std::endl;
                    }
                else if (servSock[n].second.getLocation("/") == nullptr 
                    && !servSock[n].second.getIndex().empty())
                    {// file = servRoot + file requested;
                        path = servSock[n].second.getRoot() + rqst.get_file();
                        if (!access(path.c_str(), F_OK)){
                            file = path;
                            rsp.set_status(200);
                        }else
                            rsp.set_status(404);
                        std::cout << "***> "<< file << std::endl;
                    }
                    
            }
            std::cout << rqst.get_file() << "<========" << file << "=========> "<< std::endl;
            rsp.BuildResponse(rqst, file);

            if (send(servSock[n].first.get_socket(), rsp.get_request().c_str(), strlen(rsp.get_request().c_str()), 0) == -1){
            std::cout << "error" << std::endl;
            }
        // }
        std::cout << "====" << std::endl; 
        servSock[n].first.close_sock();
        servSock.erase(servSock.begin() + n); 
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