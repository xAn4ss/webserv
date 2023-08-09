#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Socket.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"


class Webserv{
private:
    std::vector<Socket> soc;
    Config config;
public:
    Webserv(std::string s);
    ~Webserv();
    int startParsing();
    int startSockets();
    int checkSocExistance(int);
};

int Webserv::checkSocExistance(int socket){
    for (int i = 0; i < soc.size(); i++){
        std::cout << "sockets are : " << soc[i].get_socket() << std::endl;

        if (socket == soc[i].get_socket())
            return -1;
    }
    return 0;
}

int Webserv::startSockets(){
    if (config.getServers() == nullptr)
        return (1);
    int i = 0 ;
    int max_fd;
    fd_set port_fd;
    while (i < (*config.getServers()).size())    
    {
        std::cout << "Server "<< i + 1<<":\nHost: " << (*config.getServers())[i].gethost() << std::endl;
        std::cout << "Ports are: ";
        (*config.getServers())[i].printPorts();

        int x = 1;
        Socket sock;
        //  << " <<<<<<" << std::endl;

        for (int n = 0; n < ((*config.getServers())[i].getPorts()).size(); n++){
            // ((*config.getServers())[i].getPorts())[i]

            sock.setSocket(socket(AF_INET, SOCK_STREAM, 0));
            if (sock.get_socket() < 0)
            {
                std::cout << "Couldnt create socket.\n" << std::endl;
                return -1;
            }
            sockaddr_in sock_addr;
            sock_addr.sin_family = AF_INET;
            sock_addr.sin_port = htons((*config.getServers())[i].getPorts()[n]);
            sock_addr.sin_addr.s_addr = INADDR_ANY;
            // std::cout << ">>>>>> " << (*config.getServers())[i].getPorts()[n] << " <<<<<<" << std::endl;
            if (setsockopt(sock.get_socket(), SOL_SOCKET, SO_REUSEADDR, &x, sizeof(x)) == -1){
                perror("sockopt");
                return -1;
            }
            int bnd = bind(sock.get_socket(), (struct sockaddr *)&sock_addr, sizeof(sock_addr));
            if (bnd < 0)
            {
                perror("bind");
                return -1;
            }
            int lstn = listen(sock.get_socket(), 10);
            if (lstn < 0)
            {
                perror("lstn");
                return -1;
            }

            this->soc.push_back(sock);
            max_fd = std::max(max_fd, sock.get_socket());    
        }
        i++;
    }
    std::cout << "sockets size : "<< soc.size() << std::endl;
    for (std::vector<Socket>::iterator it = soc.begin(); it != soc.end(); it++)
    {
        std::cout << (*it).get_socket() << std::endl;
    }
    while (true) {
        FD_ZERO(&port_fd);
        for (int i = 0; i < soc.size(); i++){
            FD_SET(soc[i].get_socket(), &port_fd);
        }
        int slct = select(max_fd + 1, &port_fd, NULL, NULL, NULL);
        if (slct < 0){
            perror("select error:");
            return -1;
        }
        for (int i = 0; i < soc.size(); i++){
            if (FD_ISSET(soc[i].get_socket() , &port_fd))
            {
                if (checkSocExistance(soc[i].get_socket()))
                {
                    sockaddr_in soc_address;
                    socklen_t addr_len = sizeof(soc_address);
                    Socket new_sock;
                    new_sock.setSocket(accept(soc[i].get_socket(), (struct sockaddr*)&soc_address, &addr_len));
                    std::cout << "new sock : " << new_sock.get_socket() << std::endl;
                    soc.push_back(new_sock);
                    max_fd = std::max(max_fd, new_sock.get_socket());
                }else{
                    char buf[1024];
                    std::cout << "reading from  : " << soc[i].get_socket() << std::endl;
                    int rcv = read(soc[i].get_socket(), buf, sizeof(buf));
                    if (rcv < 0)
                    {
                        perror("read");
                        return -1;
                    }else if (rcv > 0){
                        soc[i].set_request(buf);
                    }else{
                        std::cout << "~~Disconnected~~" << std::endl;
                        soc[i].close_sock();
                        soc.erase(soc.begin()+i);
                    }
                }
            }
        } 
    }
        // std::cout << "~~~~ ~~~~" << std::endl;
        // std::cout << sock.get_request() << std::endl;
    return 0;
}

Webserv::Webserv(std::string s){
    config.setConfigFileName(s);
}

int Webserv::startParsing(){
    if (config.parse_config() < 0)
        return -1;
    return 0;
}

Webserv::~Webserv(){

}


int main(int ac, char **av)
{
    Socket soc;
    if (ac != 2)
    {
        std::cout << "Error:\n./webserv conf-file." << std::endl;
        return 0;
    }
    Webserv run(av[1]);
    if (run.startParsing() < 0)
    {
        std::cout << "Something is wrong in conf file" << std::endl;
        return 0;
    }
    run.startSockets();
    // if (soc.create_socket() < 0)
    //     return 0;
    // std::cout << "-> " << soc.get_request() << "." << std::endl;
    // Request request;
    // request.parse(soc.get_request());
    // std::cout << "{" << request.get_method() << "}" << std::endl;
    // std::cout << "{" << request.get_file() << "}" << std::endl;

    // Response response;
    // // response.
    // response.set_status(200);
    // config.
    soc.close_sock();
}
