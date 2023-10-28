#include "Includes/Webserv.hpp"



int main(int ac, char **av)
{
    Socket soc;
    if (ac != 2)
    {
        std::cout << "Error:\n./webserv conf-file." << std::endl;
        return 0;
    }
    Webserv run(av[1]);
    if (run.config.parse_config() < 0)
    {
        std::cout << "Something is wrong in conf file" << std::endl;
        return 0;
    }
    
    run.startSockets();
    soc.close_sock();
}
