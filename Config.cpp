#include "Includes/Config.hpp"

std::vector<Server>* Config::getServers(){
    if (servers.empty())
        return nullptr;
    return &servers;
}


void Config::setConfigFileName(std::string s){
    config_file = s;
}

int Config::check_brackets(char *s){
    std::string config = s;
    int i = 0;
    int counter = 0;
    while ((i = config.find_first_of("{}", i)) != -1)
    {
        if (config.at(i) == '{'){
            counter++;
        }else
            counter--;
        i++;
    }
    if (counter)
        return -1;
    return 0;
}

void Config::printConfig(){
    std::vector<std::string>::iterator it = configVec.begin();
    while (it != configVec.end())
    {
        std::cout << *it << std::endl;
        it++;
    }
}

void getRidOfTabs(std::vector<std::string> &s){
    std::string tmp;
    std::vector<std::string>::iterator it = s.begin();
    for (int i = 0; i < (int)s.size(); i++){
            if ((int)s[i].find_first_not_of("\t ") != -1 )
            {
                tmp = s[i].substr(s[i].find_first_not_of("\t "));
                if (s[i].front() == '\t')
                    s[i].swap(tmp);
            }
            else
                s.erase(it + i);
    }
}

int Config::confParse(){
    char s[1042];
    int fd;

    std::string buf ;
    if ((fd = open(this->config_file.c_str(), O_RDONLY)) < 0)
    {
        std::cout << "Can't open." << std::endl;
        return -1;
    }
    int rd = read(fd, s, 1042);
    if (rd > 0){
        s[rd] = '\0';
    }else{
        std::cout << "Can't read." << std::endl;
        return -1;
    }
    if (check_brackets(s) < 0)
        return -1;
    int i = 0;
    char *x = strtok(s, "\n;");
    while (x){
        configVec.push_back(x);
        x = strtok(NULL, "\n;");
    }
    int b;
    i = 0;
    
    getRidOfTabs(configVec);
    std::vector<std::string>::iterator it = configVec.begin();
    while (i < (int)    configVec.size())
    {
        int c = 0;
        b = 0;
        int hash_f = 0;
        if ((*(it + i)).front() == '#')
            hash_f = -1;
        if (!hash_f && (!strncmp(configVec[i].c_str(), "server", 6)) && ((!strncmp(configVec[i+1].c_str(), "{", 1) || configVec[i].back() == '{') || configVec[i].front() == '#'))
        {
            hash_f = 0;    
            if(configVec[i+1].back() == '{')
                i++;
            i++;
            b = i;
            while (configVec[i].back() != '}')
            {
                if (!strncmp(configVec[i].c_str(), "location", 8) && (!strncmp(configVec[i+1].c_str(), "{", 1) || configVec[i].back() == '{'))
                {
                    if(!strncmp(configVec[i+1].c_str(), "{", 1))
                        i++;
                    i++;
                    while (strncmp(configVec[i].c_str(), "}", 1))
                        i++;
                }
                i++;
            }
            c = i;
        }
        if (!hash_f)
        {
            std::vector<std::string>::iterator bgin = it +b;
            std::vector<std::string>::iterator end = it +c;
            if (parse_server(bgin, end))
                return -1;
        }
        i++;
    }
    return 0;
}

int Config::parse_server(std::vector<std::string>::iterator &b, std::vector<std::string>::iterator &e){
    std::vector<std::string> conf(b, e);
    Server serv;
    std::vector<std::string>::iterator i = conf.begin();
    int s;

    while (i != conf.end())
    {

        s = 0;
        if (!strncmp((*i).c_str(), "#", 1))
            i++;
        else if (!strncmp("host", (*i).c_str(), 4))
        {
            if (serv.setHost(splitIt(*i, s), s))
                return 1;
            i++;
        }
        else if (!strncmp("port", (*i).c_str(), 4))
        {
            if (serv.setPort(splitIt(*i, s), s))
                return 1;
            i++;
        }
        else if (!strncmp("root", (*i).c_str(), 4))
        {

            if (serv.setRoot(splitIt(*i, s), s))
                return 1;
            i++;
        }
        else if (!strncmp("index", (*i).c_str(), 5))
        {
            if (serv.setIndex(splitIt(*i, s), s))
                return 1;
            i++;
        }
        else if (!strncmp("server_name", (*i).c_str(), 11))
        {
            if (serv.setServerName(splitIt(*i, s),s))
                return 1;
            i++;
        }
        else if (!strncmp("error_page", (*i).c_str(), 10))
        {
            if (serv.setError(splitIt(*i, s), s))
                return 1;
            i++;
        }
        else if (!strncmp("autoIndex", (*i).c_str(), 9)){
            if (serv.setAutoIndex(splitIt(*i, s), s))
                return 1;
            i++;
        }
        else if(!strncmp("method", (*i).c_str(), 6)){
            if (serv.setMethods(splitIt(*i, s), s))
                return 1;
            i++;
        }
        else if (!strncmp((*i).c_str(), "location", 8) &&
         (!strncmp((*(i + 1)).c_str(), "{", 1) || ((*i).back() == '{')))
        {
            ServLocation location;
            std::vector<std::string>::iterator begin = i;
            if (!strncmp((*(i + 1)).c_str(), "{", 1))
                i++;
            while (strncmp((*i).c_str(), "}", 1))
                i++;
            if (location.processLocation(begin, i))
                return 1;
            i++;
            //push servLocation in location vector
            serv.addLocation(location);

        }
        else if (strncmp((*i).c_str(), "}", 1))
        {
            std::cout << "Wrong directive " << *i << ".." << std::endl;
            return 1;
        }
        // i++;
    }
    if (b == e)
    {
        std::cout << "error in conf" << std::endl;
        return -1;
    }
    if (serv.checkServ())
        return -1;
    this->servers.push_back(serv);
    return 0;
}
/*
host
port
root (must not be empty)
index
server_name (goes by default to localhost)
error
auto index
return
client_max_body_size
location{
    error
    index
    root
    autoindex
    return
    client_max_body_size
    methods
    cgi_path
    cgi_exec
    upload
}
*/



int Config::parse_config()
{
    if (confParse() < 0)
        return -1;
    
    // printConfig();
    int i = -1;

    while (++i < (int)servers.size())
    {
        std::cout << "host is " << this->servers[i].gethost() << std::endl;
        std::vector<ServLocation>* tmp = (this->servers[i].getLocations());        
        std::cout << "ports are ";
        this->servers[i].printPorts();
        if (tmp != nullptr){
            for (int i = 0; i < (int)tmp->size(); i++)
            {
                std::cout << "location path: "  << (*tmp)[i].getLocationPath() << std::endl;
                std::cout << "location index: "  << (*tmp)[i].getLocationIndex() << std::endl;
                if ((*tmp)[i].getLocationAutoIndex()){
                    std::cout << "location autoIndex On." << std::endl;
                }

            }
        }
        std::cout << "========= Server : "<< i+1 << " done. ==========" << std::endl;
    }
    return 0;
}

Config::Config()
{
}

Config::~Config()
{
}
