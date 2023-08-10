#ifndef SOCKET_HPP
#define SOCKET_HPP
#include <unistd.h>
#include <iostream>
#include "Request.hpp"
class Socket
{
private:
    std::string request;
    int sock;
public:
    Socket();
    ~Socket();
    void set_request(std::string req);
    std::string get_request();
    int get_socket();
    void setSocket(int);
    int create_socket(int);
    void close_sock();
    void handleRequest();
};

Socket::Socket(/* args */)
{
}

Socket::~Socket()
{
}

void Socket::handleRequest(){
    Request rqst;
    rqst.parse(request);
    std::cout << "=> " << rqst.get_method() << std::endl;
    std::cout << "=> " << rqst.get_file() << std::endl;
    std::cout << "=> " << rqst.get_http_v() << std::endl;
    // if (!rqst.get_method().compare("GET")){
    //     if (!rqst.get_file().compare("/") && )
    // }
}

void Socket::setSocket(int n){
    this->sock = n;
}

void Socket::close_sock(){
    close(sock);
}

void Socket::set_request(std::string req){
    this->request = req;
}

std::string Socket::get_request(){
    return this->request;
}
int Socket::get_socket(){
    return this->sock;
}
// int Socket::create_socket(int port){
    
//     int sock = socket(AF_INET, SOCK_STREAM, 0);
//     this->sock = sock;
//     if (sock < 0)
//     {
//         std::cout << "Couldnt create socket.\n" << std::endl;
//         return -1;
//     }
//     sockaddr_in sock_addr;
//     sock_addr.sin_family = AF_INET;
//     sock_addr.sin_port = htons(port);
//     sock_addr.sin_addr.s_addr = INADDR_ANY;
//     socklen_t s_len = sizeof(sock_addr);
//     int x = 1;
//     if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &x, sizeof(x)) == -1){
//         perror("sockopt");
//         return -1;
//     }
//     int bnd = bind(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
//     if (bnd < 0)
//     {
//         perror("bind");
//         return -1;
//     }
//     int lstn = listen(sock, 10);
//     if (lstn < 0)
//     {
//         perror("lstn");
//         return -1;
//     }
//     char buf[1024];
//     // while (1)
//     // {
//         int acc = accept(sock, (struct sockaddr*)&sock_addr, &s_len);
//         if (acc < 0)
//         {
//             perror("accept");
//             return -1;
//         }
//         int rcv = read(acc, buf, sizeof(buf));
//         if (rcv == -1)
//         {
//             perror("read");
//             return -1;
//         }
//     this->request = buf; 
//     return 0;
// }

#endif