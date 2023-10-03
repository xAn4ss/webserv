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
    std::string file;
    bool chunked;
    std::string chunkedData;
public:
    ServSock(/* args */);
    ~ServSock();
    int size();
    void remove(int);
    void addPair(std::pair<Socket, Server>);
    std::pair<Socket, Server>& operator[](int);
    void processConnection(int);
    void buildResponse(int, Response&, Request);
    void handleErrors(int, Response&, Request);
    std::string fct(std::string file, int& s);
    void buildHead(int n, Response&, Request);
    void sendResponse(int, Response);
};

void ServSock::handleErrors(int n, Response& rsp, Request rqst){

}

void ServSock::sendResponse(int n, Response rsp){
    if (chunked){
        std::string tp;
        int x = chunkedData.size();
        send(servSock[n].first.get_socket(), rsp.get_request().c_str(), rsp.get_request().size(), MSG_NOSIGNAL);
        std::cout << "§§" << rsp.get_request() << std::endl;
        while (!(tp = fct(file, x)).empty())
        {
            send(servSock[n].first.get_socket(), tp.c_str(), tp.size(), MSG_NOSIGNAL);
            std::cout << "§§" << tp << std::endl;    
            if (tp == "0\r\n\r\n"){
                send(servSock[n].first.get_socket(), tp.c_str(), tp.size(), MSG_NOSIGNAL);
                std::cout << "§§" << tp << std::endl;    
                break;
            }
        }
    }else if (send(servSock[n].first.get_socket(), rsp.get_request().c_str(), strlen(rsp.get_request().c_str()), MSG_NOSIGNAL) == -1){
    std::cout << "error" << std::endl;
    exit(0);
    /*****************/
    }
    std::cout << "§§" << rsp.get_request() << std::endl;

}

ServSock::ServSock(/* args */)
{
    i = 0;
    chunked = false;
}

std::string ServSock::fct(std::string file, int& s){
    int fd = open(file.c_str(), O_RDONLY);
    if (fd == -1){
        std::cout << "apah"<< std::endl;
        return ("-1");
    }
    lseek(fd, i, SEEK_SET);
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cout << "error fcntl" << std::endl;
        exit(0);
    }
    int chunk;
    if (s < 2048)
        chunk = 2048 - s;
    else
        chunk = 2048;
    char buf[chunk];
    int size = read(fd, buf, chunk);
    if (size == 0){
        i = 0;
        close(fd);
        return ("0\r\n\r\n");
    }
    else if (size == -1)
    {
        close(fd);
        return ("");
    }
    std::string tmp(buf, size);
    i += size;
    s -= size;
    close(fd);
    return tmp;
}

/*
buildResponse => buildHeader
              => buildBody
*/

void ServSock::buildHead(int n, Response& rsp, Request rqst){

    rsp + (rqst.get_http_v() + " ");
    rsp + std::to_string(rsp.get_status());
    if (rsp.get_status() == 200)
        rsp + " OK\r\n";
    else if (rsp.get_status() == 301)
        rsp + (" Moved Permantely\nLocation: " + file +"\n");
    else{
        file = servSock[n].second.getErrorPath();
        if (rsp.get_status() == 404){
            file += std::to_string(rsp.get_status()) + ".html";
            rsp + " Not Found";
        }else if (rsp.get_status() == 403){
            file += std::to_string(rsp.get_status()) + ".html";
            rsp + " Forbidden";
        }else//check for other error codes
            rsp + " undefined";
        rsp + "\r\n";
    }
}
void setContentLength(Response& rsp, std::string& chunkedData, std::string file){
    std::ifstream s(file, std::ios::binary);
    std::vector<char> vec;
    char c;
    while (s.get(c))
        vec.push_back(c);
    std::string tmp(vec.begin(), vec.end());
    chunkedData = tmp;
    s.close();
    rsp + ("Content-Length: " + std::to_string(tmp.size()) + "\r\n\r\n");
}

void ServSock::buildResponse(int n, Response& rsp, Request rqst){
    chunked = 0;
    buildHead(n, rsp, rqst);
    if (rsp.get_status() == 301)
        return;
    if ((file.length() - file.find(".css")) == 4)
        rsp + "Content-Type: text/css\n";
    else if((file.length() - file.find(".html")) == 5)
        rsp + "Content-Type: text/html\n";
    else if ((file.length() - file.find(".jpeg")) == 5){
        chunked = 1;
        rsp + "Content-Type: image/jpeg\r\n";
        setContentLength(rsp, chunkedData, file);
    }
    else if ((file.length() - file.find(".png")) == 4)
    {
        chunked = 1;
        rsp + "Content-Type: image/png\r\n";
        setContentLength(rsp, chunkedData, file);
    }
    else if ((file.length() - file.find(".mp4")) == 4){
        chunked = 1;
        rsp + "Content-Type: video/mp4\r\n";
        setContentLength(rsp, chunkedData, file);
    }

    if (!chunked){
        std::string fileContent;
        std::cout << file << std::endl;
        std::ifstream f(file);
        std::string tm;
        while (getline(f, tm)){
            fileContent += (tm);
        }
        rsp + ("Content-Length: " + std::to_string(fileContent.length())+ "\r\n\r\n");
        rsp + fileContent;
    }
}

void ServSock::processConnection(int n){
    Request     rqst;
    Response    rsp;
    rqst.parse(servSock[n].first.get_request());
        std::cout << "=== " << rqst.get_method() << std::endl;
        std::cout << "=== " << rqst.get_file() << std::endl;
    rsp.set_status(200);
    int autIndexed = 0;
    if (!rqst.get_method().compare("GET"))
    {
        // if dir => check index of that directory in server lcoation
        // else if file 
        //      => locationroot + file;
        //      => serverRoot + file;
            if (rqst.get_file() == "/favicon.ico")
                return;
            if (rqst.get_file().compare("/"))
                rqst.setFile(rqst.get_file().substr(1, strlen(rqst.get_file().c_str())));
            struct stat slatt;
            if (!stat(rqst.get_file().c_str(), &slatt))
            {
                if (S_ISDIR(slatt.st_mode))
                {
                    std::cout << "dir "<< std::endl;
                    if (servSock[n].second.getLocation(rqst.get_file()) == nullptr 
                        && !servSock[n].second.getIndex().empty()){
                            struct stat t;
                            if (rqst.get_file().compare("/")){
                                rsp.set_status(403);
                                std::cout << " ===========  "<< std::endl;
                            }
                            else
                                file = servSock[n].second.getIndex();
                    }
                    else if (servSock[n].second.getLocation(rqst.get_file()))
                    {    // when u give a folder path u get index to that location/folder
                        if (servSock[n].second.getAutoIndex())
                        {
                            autIndexed = 1;
                            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
                            response += "<html><head><title>Index of ";
                            response += "auto index test";
                            response += "</title></head><body><h1>Index of ";
                            response += "AutoIndex Test";
                            response += "</h1><ul>";
                            response += "<li><a href=\"";
                            response += "test/test.html";
                            response += "\">";
                            response += "Testing ...";
                            response += "</a></li>";
                            response += "</ul></body></html>";
                            rsp + response;
                        }
                            std::cout << ">>>>>>>" << rsp.get_request() <<std::endl;

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
                            std::cout << "\\\\\\\\\\\\\\\\";
                            file = path;
                            rsp.set_status(200);
                        }else
                            rsp.set_status(404);
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
                    }
                    
            }
            if (!autIndexed)
                buildResponse(n, rsp, rqst);
            sendResponse(n, rsp);
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