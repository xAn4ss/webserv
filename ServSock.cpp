#include "Includes/ServSock.hpp"
#include "methods/Handler.hpp"

void ServSock::sendResponse(int n, Response rsp){
    if (chunked){
        std::string tp;
        int x = chunkedData.size();
        send(servSock[n].first.get_socket(), rsp.get_request().c_str(), rsp.get_request().size(), 0);
        // std::cout << "§§" << rsp.get_request() << std::endl;
        while (!(tp = fct(file, x)).empty())
        {
            send(servSock[n].first.get_socket(), tp.c_str(), tp.size(), 0);
            // std::cout << "§§" << tp << std::endl;
            if (tp == "0\r\n\r\n"){
                send(servSock[n].first.get_socket(), tp.c_str(), tp.size(), 0);
                // std::cout << "§§" << tp << std::endl;    
                break;
            }
        }
    }else if (send(servSock[n].first.get_socket(), rsp.get_request().c_str(), strlen(rsp.get_request().c_str()), 0) == -1){
    std::cout << "error" << std::endl;
    exit(0);
    /*****************/
    }
    // std::cout << "§§" << rsp.get_request() << std::endl;

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
        }else if (rsp.get_status() == 501){
            file += std::to_string(rsp.get_status()) + ".html";
            rsp + " Not Implemented";
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

std::string buildAutoIndex(std::string path){
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    response += "<html><head><title>Index of ";
    response += "auto index test";
    response += "</title></head><body><h1>Index of ";
    response += path;
    response += "</h1><ul>";
    struct dirent *dir;
    DIR *d = opendir(path.c_str());
    while ((dir = readdir(d)) != NULL)
    {
        response += "<li><a href=\"";
        response += "../" + path + dir->d_name;
        response += "\">";
        response += dir->d_name;
        response += "</a></li>";
        std::cout << ">>>>>>>" << dir->d_name <<std::endl;                                
    }
    response += "</ul></body></html>";
    return response;
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
                    if (servSock[n].second.getLocation(rqst.get_file()+"/"))
                    {
                        file = rqst.get_file() + "/";
                        rsp.set_status(301);
                    }
                    else if (servSock[n].second.getLocation(rqst.get_file()) == nullptr 
                        ){
                            if (rqst.get_file().compare("/")){
                                rsp.set_status(403);
                            }
                            else
                            {
                                if (servSock[n].second.getAutoIndex() == true
                                     && servSock[n].second.getIndex().empty()){
                                        std::cout << " ======*====  "<< std::endl;
                                        autIndexed = 1;
                                        rsp + buildAutoIndex(servSock[n].second.getRoot());
                                }else{
                                
                                    if (servSock[n].second.getMethods()["GET"] > 0)
                                    {
                                        file = servSock[n].second.getIndex();
                                        rsp.set_status(200);
                                    }
                                    else
                                        rsp.set_status(501);
                                }
                            }
                    }
                    else if (servSock[n].second.getLocation(rqst.get_file()))
                    {    // when u give a folder path u get index to that location/folder
                        ServLocation *tmp = servSock[n].second.getLocation(rqst.get_file());
                        if (tmp->getLocationAutoIndex() && tmp->getLocationIndex().empty())
                        {
                            autIndexed = 1;
                            rsp + buildAutoIndex(tmp->getLocationRoot());
                        }
                        if ((*servSock[n].second.getLocation(rqst.get_file())).getLocationMethods()["GET"] > 0)
                        {
                            file = (*servSock[n].second.getLocation(rqst.get_file())).getLocationIndex();
                            rsp.set_status(200);
                        }else
                            rsp.set_status(501);
                    }else
                        rsp.set_status(403);
                }else{
                    std::cout << "file" << std::endl;
                    std::string path;
                    if (!access(rqst.get_file().c_str(), F_OK))
                    { // if path requested exists file = path
                        std::cout << "=> " << rqst.get_file().substr(0, rqst.get_file().find_last_of("/")+1) << std::endl;
                        if (servSock[n].second.getLocation(rqst.get_file().substr(0, rqst.get_file().find_last_of("/")+1))){
                                if (servSock[n].second.getLocation(rqst.get_file().substr(0, rqst.get_file().find_last_of("/")+1))->getLocationMethods()["GET"] > 0){
                                    file = rqst.get_file();
                                    rsp.set_status(200);
                                }
                                else
                                    rsp.set_status(501);
                        }else if (servSock[n].second.getMethods()["GET"] < 1)
                            rsp.set_status(501);
                        else{
                            file = rqst.get_file();
                            rsp.set_status(200);
                        }
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
    }else {
        Handler handler(rqst);
        handler.handleMethod();
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
