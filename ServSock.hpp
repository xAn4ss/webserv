#ifndef SERVSOCK_HPP
#define SERVSOCK_HPP

#include "Socket.hpp"
#include "Server.hpp"
#include <algorithm>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
class ServSock
{
private:
    std::vector<std::pair<Socket, Server>> servSock;
    int i;
    std::string t;
public:
    ServSock(/* args */);
    ~ServSock();
    int size();
    void remove(int);
    void addPair(std::pair<Socket, Server>);
    std::pair<Socket, Server>& operator[](int);
    void processConnection(int);
    void buildResponse(int, std::string, Response&, Request);
    void handleErrors(int, Response&, Request);
    std::string fct(std::string file);
};
void ServSock::handleErrors(int n, Response& rsp, Request rqst){

}

ServSock::ServSock(/* args */)
{
    i = 0;
}

std::string ServSock::fct(std::string file){
    std::cout << "*/**/**/*/*/*/***/*/" << std::endl;
    int fd = open(file.c_str(), O_RDONLY);
    if (fd == -1){
        std::cout << "apah"<< std::endl;
        exit(0);
    }
    lseek(fd, i, SEEK_SET);
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cout << "error fcntl" << std::endl;
        exit(0);
    }
    
    char buf[2048];
    int size = read(fd, buf, 2048);
    if (size == 0){
        std::cout << "plplpl" << std::endl;
        i = 0;
        close(fd);
        return ("0\r\n\r\n");
    }
    else if (size == -1)
    {
        std::cout << "plp-1lpl" << std::endl;
        close(fd);
        exit(0);
    }
    std::string tmp(buf, size);
    i += size;
    close(fd);
    // std::cout << i <<" ====> "<< tmp << std::endl;
    return tmp;
}


void ServSock::buildResponse(int n, std::string file, Response& rsp, Request rqst){
    std::cout << file << " ===== " << rsp.get_status() << std::endl;
    rsp + (rqst.get_http_v() + " ");
    rsp + std::to_string(rsp.get_status());
    if (rsp.get_status() == 200)
        rsp + " OK\r\n";
    else if (rsp.get_status() == 301)
        rsp + (" Moved Permantely\nLocation: " + file +"\n");
    else{
        std::cout << " ========> " << std::endl;
        file = servSock[n].second.getErrorPath();
        if (rsp.get_status() == 404){
            file += std::to_string(rsp.get_status()) + ".html";
            rsp + " Not Found";
        }else if (rsp.get_status() == 403){
            file += std::to_string(rsp.get_status()) + ".html";
            rsp + " Forbidden";
        }else
            rsp + " undefined";
        rsp + "\r\n";
        std::cout << file << " ==> "<< rsp.get_request() << std::endl;
    }
    // rsp + "Content-Type: ";
    std::cout << file.length() - file.find(".html") << std::endl;
    if ((file.length() - file.find(".css")) == 4)
        rsp + "Content-Type: text/css\n";
    else if((file.length() - file.find(".html")) == 5)
        rsp + "Content-Type: text/html\n";
    else if ((file.length() - file.find(".jpeg")) == 5){
        std::ifstream s(file, std::ios::binary);
        std::vector<char> vec;
        char c;
        while (s.get(c))
            vec.push_back(c);
        std::string tmp(vec.begin(), vec.end());
        s.close();
        rsp + ("Content-Length: " + std::to_string(tmp.size()) + "\r\n");
        rsp + "Content-Type: image/jpeg\r\n\r\n";
        rsp + tmp;
        std::cout << "**" << rsp.get_request() << std::endl;
        return;
    }
    else if ((file.length() - file.find(".png")) == 4)
    {
        // std::cout << "... "<< t << std::endl;
        std::ifstream s(file, std::ios::binary);
        std::vector<char> vec;
        char c;
        while (s.get(c))
            vec.push_back(c);
        std::string tmp(vec.begin(), vec.end());
        s.close();
        rsp + ("Content-Length: " + std::to_string(tmp.size()) + "\n");
        rsp + "Content-Type: image/png\r\n\r\n";
        std::string tp;
        while ((tp = fct(file)) != "0\r\n\r\n")
        {
            t += tp;
        }
        t += "0\r\n\r\n";
        std::cout << ">> >>" << t << std::endl;

        rsp + t;
        // t += fct(file);
        std::cout << "**" << rsp.get_request() << std::endl;
        return;
    }
    std::string fileContent;
    std::ifstream f(file);
    std::string tm;
    while (getline(f, tm)){
        fileContent += (tm);
    }
    rsp + ("Content-Length: " + std::to_string(fileContent.length())+ "\r\n\r\n");
    rsp + fileContent;
    std::cout << "§§" << rsp.get_request() << std::endl;
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
            if (rqst.get_file() == "/favicon.ico")
                return;
            std::string file;
            if (rqst.get_file().compare("/"))
                rqst.setFile(rqst.get_file().substr(1, strlen(rqst.get_file().c_str())));
            struct stat slatt;
            // file = rqst.get_file();
            if (!stat(rqst.get_file().c_str(), &slatt))
            {
                std::cout <<stat(rqst.get_file().c_str(), &slatt)<< "-*-*-*--*-*-" << rqst.get_file() << "*-*-*-*--*-*--*-" << std::endl;
                if (S_ISDIR(slatt.st_mode))
                {
                    std::cout << "dir "<< std::endl;
                    if (servSock[n].second.getLocation(rqst.get_file()) == nullptr 
                        && !servSock[n].second.getIndex().empty()){
                            struct stat t;
                            std::cout << stat(rqst.get_file().c_str(), &t) << std::endl;
                            if (!stat(rqst.get_file().c_str(), &t) && rqst.get_file().compare("/")){
                                rsp.set_status(403);
                                std::cout << ">>>>";
                            }
                            else{
                                rsp.set_status(200);
                                file = servSock[n].second.getIndex();
                            }
                        std::cout << "... " << file << std::endl; 
                    }
                    else if (servSock[n].second.getLocation(rqst.get_file()))
                    {    // when u give a folder path u get index to that location/folder
                        file = (*servSock[n].second.getLocation(rqst.get_file())).getLocationIndex();
                        rsp.set_status(200);
                    }else if (servSock[n].second.getLocation(rqst.get_file()+"/"))
                    {
                        file = rqst.get_file() + "/";
                        rsp.set_status(301);
                    }else
                        rsp.set_status(403);
                }else{
                    std::cout << "file" << std::endl;
                    std::string path;
                    if (!access(rqst.get_file().c_str(), F_OK))
                    { // if path requested exists file = path
                        std::cout << rqst.get_file().substr(0, rqst.get_file().find_last_of("/")+1) << std::endl;
                        // if (servSock[n].second.getLocation(rqst.get_file().substr(0, rqst.get_file().find_last_of("/")+1))){
                            file = rqst.get_file();
                            rsp.set_status(200);
                        // }else if (rqst.get_file().find(servSock[n].second.getErrorPath()) != -1){
                        //     file = rqst.get_file();
                        //     rsp.set_status(200);
                        // }else
                        //     rsp.set_status(403);
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
            buildResponse(n, file, rsp, rqst);
            // std::cout << rsp.get_request() << "<========" << file << "=========> "<< std::endl;
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