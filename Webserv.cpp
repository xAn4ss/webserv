#include "Includes/Webserv.hpp"
int Webserv::checkSocExistance(int socket){
    for (size_t i = 0; i < soc.size(); i++){
        std::cout << "sockets are : " << soc[i].get_socket() << std::endl;

        if (socket == soc[i].get_socket())
            return -1;
    }
    return 0;
}

int Webserv::startSockets(){
    if (config.getServers() == NULL)
        return (1);
    
    int i = 0 ;
    int max_fd;
    fd_set port_fd;
    std::vector<int> tmp_fd;
    ServSock servSock;
    while (i < (int)(*config.getServers()).size())    
    {
        int x = 1;
        Socket sock;
        for (size_t n = 0; n < ((*config.getServers())[i].getPorts()).size(); n++){
            tmp_fd.push_back(socket(AF_INET, SOCK_STREAM, 0));
            sock.setSocket(tmp_fd.back());
            if (sock.get_socket() < 0)
            {
                std::cout << "Couldnt create socket.\n" << std::endl;
                return -1;
            }
            sockaddr_in sock_addr;
            sock_addr.sin_family = AF_INET;
            sock_addr.sin_port = htons((*config.getServers())[i].getPorts()[n]);
            sock_addr.sin_addr.s_addr = INADDR_ANY;
            if (setsockopt(sock.get_socket(), SOL_SOCKET, SO_REUSEADDR, &x, sizeof(x)) == -1){
                perror("sockopt");
                return -1;
            }
            // if (setsockopt(sock.get_socket(), SOL_SOCKET, SO_REUSEPORT, &x, sizeof(x)) == -1){
            //     perror("sockopt");
            //     return -1;
            // }
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
    for (std::vector<Socket>::iterator sok = soc.begin(); sok != soc.end();){
        for (std::vector<Server>::iterator srv = (*config.getServers()).begin(); srv != (*config.getServers()).end(); srv++){
            for (size_t i = 0; i < (*srv).getPorts().size() ; i++)
            {
                servSock.addPair(std::make_pair((*sok), (*srv)));
                sok++;
            }
        }
    }
    while (true) {
        FD_ZERO(&port_fd);
        for (int i = 0; i < servSock.size(); i++){
            FD_SET(servSock[i].first.get_socket(), &port_fd);
        }
        int slct = select(max_fd + 1, &port_fd, NULL, NULL, NULL);
        if (slct < 0){
            perror("select error:");
            return -1;
        }
        for (int i = 0; i < servSock.size(); i++){
            if (FD_ISSET(servSock[i].first.get_socket() , &port_fd))
            {
                if (std::find(tmp_fd.begin(), tmp_fd.end(), servSock[i].first.get_socket()) != tmp_fd.end())
                {
                    sockaddr_in soc_address;
                    socklen_t addr_len = sizeof(soc_address);
                    Socket new_sock;
                    new_sock.setSocket(accept(servSock[i].first.get_socket(), (struct sockaddr*)&soc_address, &addr_len));
                    std::cout << "new sock : " << servSock[i].first.get_socket() << std::endl;
                    servSock.addPair(std::make_pair(new_sock, servSock[i].second));
                    max_fd = std::max(max_fd, new_sock.get_socket());
                }else{
                    char buf[1024];
                    std::cout << "reading from  : " << servSock[i].first.get_socket() << std::endl;
                    int rcv = read(servSock[i].first.get_socket(), buf, sizeof(buf));
                    if (rcv < 0)
                    {
                        perror("read");
                        return -1;
                    }else if (rcv > 0){
                        buf[rcv] = '\0';
                        servSock[i].first.set_request(buf);
                        std::cout << servSock[i].first.get_request() << std::endl;
                        servSock.processConnection(i);
                    }else{
                        std::cout << "~~Disconnected~~" << std::endl;
                        servSock[i].first.close_sock();
                        servSock.remove(i);
                    }
                }
            }
        } 
    }
    //////should close sockets
    return 0;
}

Webserv::Webserv(std::string s){
    config.setConfigFileName(s);
}

Webserv::~Webserv(){

}
