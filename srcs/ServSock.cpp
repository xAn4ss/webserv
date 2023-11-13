#include "../Includes/ServSock.hpp"
#include "../methods/Handler.hpp"
#include "../cgi/cgi_handler.hpp"


void ServSock::sendResponse(int n, Response rsp)
{
    if (chunked)
    {
        std::string tp;
        int x = chunkedData.size();
        send(servSock[n].first.get_socket(), rsp.get_response().c_str(), rsp.get_response().size(), 0);
        // std::cout << "§§" << rsp.get_request() << std::endl;
        while (!(tp = fct(file, x)).empty())
        {
            send(servSock[n].first.get_socket(), tp.c_str(), tp.size(), 0);
            // std::cout << "§§" << tp << std::endl;
            if (tp == "0\r\n\r\n")
            {
                send(servSock[n].first.get_socket(), tp.c_str(), tp.size(), 0);
                break;
            }
        }
    }
    else if (send(servSock[n].first.get_socket(), rsp.get_response().c_str(), strlen(rsp.get_response().c_str()), 0) == -1)
    {
        std::cout << "error" << std::endl;
        exit(0);
        /*****************/
    }
   
}

ServSock::ServSock(/* args */)
{
    i = 0;
    chunked = false;
}

std::string ServSock::fct(std::string file, int &s)
{
    int fd = open(file.c_str(), O_RDONLY);
    std::cout << "]]]]]>>> " << file << std::endl;
    if (fd == -1)
    {
        std::cout << "apah" << std::endl;
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
    if (size == 0)
    {
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

void ServSock::buildHead(int n, Response &rsp, Request rqst)
{

    rsp + (rqst.get_http_v() + " ");
    std::ostringstream s;
    s << rsp.get_status();
    rsp + s.str();
    // rsp + std::to_string(rsp.get_status());
    if (rsp.get_status() == 200)
        rsp + " OK\r\n";
    else if (rsp.get_status() == 301)
        rsp + (" Moved Permantely\nLocation: " + file + "\n");
    else
    {
        file = servSock[n].second.getErrorPath();
        if (rsp.get_status() == 404)
        {
            file += s.str() + ".html";
            rsp + " Not Found";
        }
        else if (rsp.get_status() == 403)
        {
            file += s.str() + ".html";
            rsp + " Forbidden";
        }
        else if (rsp.get_status() == 501)
        {
            // check error code
            file += s.str() + ".html";
            rsp + " Not Implemented";
        }
        else // check for other error codes
            rsp + " undefined";
        rsp + "\r\n";
    }
}

void setContentLength(Response &rsp, std::string &chunkedData, std::string file, int size)
{
    std::ifstream s(file.c_str(), std::ios::binary);
    std::vector<char> vec;
    char c;
    while (s.get(c))
        vec.push_back(c);
    std::string tmp(vec.begin(), vec.end());
    chunkedData = tmp;
    s.close();
    std::ostringstream len;
    len << (int)tmp.size();
    if (size != -1)
    {   size *= 1000;
        chunkedData.substr(0, size) ;
        std::stringstream s;
        s << (size*1000);
        rsp + ("Content-Length: " + s.str() + "\r\n\r\n");

    }else
        rsp + ("Content-Length: " + len.str() + "\r\n\r\n");
}

void ServSock::buildResponse(int n, Response &rsp, Request rqst)
{
    chunked = 0;
    std::cout << "====>>" << rqst.get_file() << std::endl;
    struct stat slatt;
    if (!stat(rqst.get_file().c_str(), &slatt)){
        if (S_ISDIR(slatt.st_mode)){

        }else{
            
        }

    }
    if (servSock[n].second.getLocation(rqst.get_file()) != NULL)
    {
        ServLocation *tmp = servSock[n].second.getLocation(rqst.get_file());

        if (tmp->getLocationIsCgi()){
            std::string cgi = rsp.get_response();
            std::cout << "*/*/*/*/*/*"<< cgi <<"/*/**/*/**/*/" << std::endl;
            rsp.set_response("");
            buildHead(n, rsp, rqst);
            rsp + cgi;
            return;
        }

    }
    if (!stat(rqst.get_file().c_str(), &slatt) && !S_ISDIR(slatt.st_mode)){
        if (servSock[n].second.getLocation(rqst.get_file()) == NULL )
            rqst.setFile("/");
        ServLocation *tmp = servSock[n].second.getLocation(rqst.get_file());
        if (tmp->getLocationIsCgi()){
            std::string cgi = rsp.get_response();
            std::cout << "==========="<< cgi << "============" << std::endl;
            rsp.set_response("");
            buildHead(n, rsp, rqst);
            rsp + cgi;
            return;
        }
    }
    buildHead(n, rsp, rqst);
    if ((rsp.get_status() / 100) == 3)
    {
        std::cout << ">>>>> " << rsp.get_response() << std::endl;
        return;
    }
    // pdf/xml png/jpg/jpeg 
    if ((file.length() - file.find(".css")) == 4)
        rsp + "Content-Type: text/css\n";
    else if ((file.length() - file.find(".html")) == 5)
        rsp + "Content-Type: text/html\n";   
    else if ((file.length() - file.find(".js")) == 3){
        chunked = 1;
        rsp + "Content-Type: application/javascript";
        setContentLength(rsp, chunkedData, file, servSock[n].second.getClientMaxBodySize());
    }
    else if ((file.length() - file.find(".pdf")) == 4){
        chunked = 1;
        rsp + "Content-Type: application/pdf";
        setContentLength(rsp, chunkedData, file, servSock[n].second.getClientMaxBodySize());
    }
    else if ((file.length() - file.find(".jpeg")) == 5)
    {
        chunked = 1;
        rsp + "Content-Type: image/jpeg\r\n";
        setContentLength(rsp, chunkedData, file, servSock[n].second.getClientMaxBodySize());
    }
    else if ((file.length() - file.find(".png")) == 4)
    {
        chunked = 1;
        rsp + "Content-Type: image/png\r\n";
        setContentLength(rsp, chunkedData, file, servSock[n].second.getClientMaxBodySize());
    }
    else if ((file.length() - file.find(".mp4")) == 4)
    {
        chunked = 1;
        rsp + "Content-Type: video/mp4\r\n";
        setContentLength(rsp, chunkedData, file, servSock[n].second.getClientMaxBodySize());
    }else
        rsp + "Content-Type: text/plain\n";
    // Not chunked request
    if (!chunked)
    {
        std::string fileContent;
        std::cout << file << std::endl;
        std::ifstream f(file.c_str());
        std::string tm;
        while (getline(f, tm))
        {
            fileContent += (tm);
        }
        std::ostringstream tmp;
        tmp << (int)fileContent.length();
        if (servSock[n].second.getClientMaxBodySize() != -1){
            std::stringstream l;
            l << (servSock[n].second.getClientMaxBodySize() * 1000);
            rsp + ("Content-Length: " + l.str() + "\r\n\r\n");
            fileContent.substr(0, servSock[n].second.getClientMaxBodySize());
        }else
            rsp + ("Content-Length: " + tmp.str() + "\r\n\r\n");
        rsp + fileContent;
    }
}

std::string buildAutoIndex(std::string path)
{
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
        std::cout << ">>>>>>>" << dir->d_name << std::endl;
    }
    response += "</ul></body></html>";
    return response;
}

void ServSock::processConnection(int n)
{
    Request rqst;
    Response rsp;
    rqst.parse(servSock[n].first.get_request());
    std::cout << "=== " << rqst.get_method() << std::endl;
    std::cout << "=== " << rqst.get_file() << std::endl;
    std::cout << "=== " << rqst.get_body() << std::endl;
    rsp.set_status(200);
    int autIndexed = 0;
    chunked = 0;
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
                std::cout << "dir " << std::endl;
                if (servSock[n].second.getLocation(rqst.get_file() + "/"))
                {
                    file = rqst.get_file() + "/";
                    rsp.set_status(301);
                }
                else if (servSock[n].second.getLocation(rqst.get_file()) == NULL)
                {
                    if (rqst.get_file().compare("/"))
                    {
                        rsp.set_status(403);
                    }
                    else
                    {
                        if (servSock[n].second.getAutoIndex() == true && servSock[n].second.getIndex().empty())
                        {
                            std::cout << " ======*====  " << std::endl;
                            autIndexed = 1;
                            rsp + buildAutoIndex(servSock[n].second.getRoot());
                        }
                        else
                        {

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
                { // when u give a folder path u get index to that location/folder
                    ServLocation *tmp = servSock[n].second.getLocation(rqst.get_file());
                    if (tmp->getLocationIsRedirected() == true)
                    {
                        file = tmp->getLocationRedirPath();
                        rsp.set_status(tmp->getLocationRedirCode());
                    }
                    else if (tmp->getLocationIsCgi() == true){
                        std::cout << tmp->getLocationCgiFile() << std::endl;
                        //hna CGI
                        int inputpipe[2];
                        int outputpipe[2];
                        if (pipe(inputpipe) == -1 || pipe(outputpipe) == -1){
                            std::cerr << "failed to open pipes" << std::endl;
                            return;
                        }


                        pid_t child_pid = fork();
                        if (child_pid == -1){
                            std::cerr << "Forking failed" << std::endl;
                            return ;
                        }
                        else if (child_pid == 0){
                            close(inputpipe[1]);
                            close(outputpipe[0]);

                            dup2(inputpipe[0], 0);
                            dup2(outputpipe[1], 1);
                            //execute CGI script
                            //std::string cgi_path =  "/home/an4ss/Desktop/webserv_saved/"+tmp->getLocationCgiFile();
                            //char *cpath[] = {const_cast<char*>("/home/an4ss/Desktop/webserv_saved/cgi-bin/php-cgi"),const_cast<char *>(cgi_path.c_str()), NULL};
                            char wor[256];
                            getcwd(wor, 256);
                            std::string cgi_path = wor;
                            std::string script_path = wor;
                            cgi_path += "/";
                            cgi_path += tmp->getLocationCgiFile();
                            script_path += "/"; 
                            script_path += tmp->getLocationCgiPath();
                            std::cerr << ">>>>>>>>>>>>CGI PATH : " << cgi_path << std::endl;
                            char *cpath[] = {const_cast<char*>(script_path.c_str()),const_cast<char *>(cgi_path.c_str()), NULL};
                            //std::cerr << "CGI PATH : " << cgi_path << std::endl;
                            //std::cerr << "dkhl hna \n" << std::endl;
                            //std::cerr << "CGI PATH : " << cgi_path << std::endl;
                            std::map<std::string, std::string> requestMap = parseRequestHeader(rqst, n, cgi_path);
                            //std::cerr << "hnna hnaaaaaaaaaaaaaaa" << std::endl;
                            char** env_var = convertToEnvp(requestMap);//convert parsed request
                            //for (int i = 0; env_var[i] != NULL; ++i) {
                            //    std::cerr << "env[" << i << "]: " << env_var[i] << std::endl;
                            //}
                            for (int i = 0; cpath[i] != NULL; ++i) {
                                std::cerr << "cpath[" << i << "]: " << cpath[i] << std::endl;
                            }
                            if (execve(cpath[0],cpath, env_var) == -1)
                            {
                            // to be modified
                            perror("execvp");
                            return ;
                            }
                            std::cerr << "###########################" << std::endl;
                            exit(1);
                        }else {
                            //in parent process
                            close(inputpipe[0]);
                            close(outputpipe[1]);

                            //Debugging ended here
                            write(inputpipe[1], servSock[n].first.get_request().c_str(), servSock[n].first.get_request().size());
                            close(inputpipe[1]);
                            rsp.set_status(200);
                            char buf[1024];
                            ssize_t bytes;
                            std::string cgi_response;
                            while ((bytes = read(outputpipe[0], buf, sizeof(buf))) > 0)
                            {
                                cgi_response.append(buf, bytes);
                                //handle CGI script, senf it as HTTP response
                            }
                            close(outputpipe[0]);
                            std::string httpResponse;
                            //httpResponse += "Content-Type: text/html\r\n";
                            //httpResponse += "Content-Length: " + std::to_string(cgiOutput.size()) + "\r\n";
                            //httpResponse += "\r\n";
                            httpResponse += cgi_response;
                            ///+/ For demonstration, you can send the HTTP response to the client or display it
                            std::cerr << "HTTPS RESPONSE ====" << std::endl;
                            std::cerr << httpResponse << std::endl;
                            std::cerr << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
                            rsp + httpResponse;
                            buildResponse(n, rsp, rqst);
                            
                            std::cerr << rsp.get_response() << std::endl;
                        }
                        std::cout << "..." << std::endl;
                    }
                    else
                    {
                        if (tmp->getLocationAutoIndex() && tmp->getLocationIndex().empty())
                        {
                            autIndexed = 1;
                            rsp + buildAutoIndex(tmp->getLocationRoot());
                        }
                        if ((*servSock[n].second.getLocation(rqst.get_file())).getLocationMethods()["GET"] > 0)
                        {
                            file = (*servSock[n].second.getLocation(rqst.get_file())).getLocationIndex();
                            rsp.set_status(200);
                        }
                        else
                            rsp.set_status(501);
                    }
                }
                else
                    rsp.set_status(403);
            }
            else
            {
                std::cout << "file" << std::endl;
                std::string path;
                if (!access(rqst.get_file().c_str(), F_OK))
                { // if path requested exists file = path
                    std::cout << "=> " << rqst.get_file().substr(0, rqst.get_file().find_last_of("/") + 1) << std::endl;
                    if (servSock[n].second.getLocation(rqst.get_file().substr(0, rqst.get_file().find_last_of("/") + 1)))
                    {
                        if (servSock[n].second.getLocation(rqst.get_file().substr(0, rqst.get_file().find_last_of("/") + 1))->getLocationMethods()["GET"] > 0)
                        {
                            file = rqst.get_file();
                            rsp.set_status(200);
                        }
                        else
                            rsp.set_status(501);
                    }
                    else if (servSock[n].second.getMethods()["GET"] < 1)
                        rsp.set_status(501);
                    else
                    {
                        ServLocation *tmp = servSock[n].second.getLocation("/");
                        if (servSock[n].second.getLocation("/")->getLocationIsCgi() == true){
                            std::cout << "-*-*-*-*-*-*-*-*-*-" << std::endl;
                        int inputpipe[2];
                        int outputpipe[2];
                        if (pipe(inputpipe) == -1 || pipe(outputpipe) == -1){
                            std::cerr << "failed to open pipes" << std::endl;
                            return;
                        }


                        pid_t child_pid = fork();
                        if (child_pid == -1){
                            std::cerr << "Forking failed" << std::endl;
                            return ;
                        }
                        else if (child_pid == 0){
                            close(inputpipe[1]);
                            close(outputpipe[0]);

                            dup2(inputpipe[0], 0);
                            dup2(outputpipe[1], 1);
                            //execute CGI script
                            //std::string cgi_path =  "/home/an4ss/Desktop/webserv_saved/"+tmp->getLocationCgiFile();
                            //char *cpath[] = {const_cast<char*>("/home/an4ss/Desktop/webserv_saved/cgi-bin/php-cgi"),const_cast<char *>(cgi_path.c_str()), NULL};
                            char wor[256];
                            getcwd(wor, 256);
                            std::string cgi_path = wor;
                            std::string script_path = wor;
                            cgi_path += tmp->getLocationCgiFile();
                            script_path += "/" + tmp->getLocationCgiPath();
                            std::cerr << ">>>>>>>>>>>>CGI PATH : " << cgi_path << std::endl;
                            std::cerr << ">>>>>>>>>>>>rqst file : " << rqst.get_file() << std::endl;
                            char *cpath[] = {const_cast<char*>(script_path.c_str()),const_cast<char *>(cgi_path.c_str()), NULL};
                            //std::cerr << "CGI PATH : " << cgi_path << std::endl;
                            //std::cerr << "dkhl hna \n" << std::endl;
                            //std::cerr << "CGI PATH : " << cgi_path << std::endl;
                            rqst.setFile("/");
                            std::map<std::string, std::string> requestMap = parseRequestHeader(rqst, n, cgi_path);
                            //std::cerr << "hnna hnaaaaaaaaaaaaaaa" << std::endl;
                            char** env_var = convertToEnvp(requestMap);//convert parsed request
                            for (int i = 0; env_var[i] != NULL; ++i) {
                               std::cerr << "==>" << "env[" << i << "]: " << env_var[i] << std::endl;
                            }
                            for (int i = 0; cpath[i] != NULL; ++i) {
                                std::cerr << "cpath[" << i << "]: " << cpath[i] << std::endl;
                            }
                            if (execve(cpath[0],cpath, env_var) == -1)
                            {
                            // to be modified
                            perror("execvp");
                            return ;
                            }
                            std::cerr << "###########################" << std::endl;
                            exit(1);
                        }else {
                            //in parent process
                            close(inputpipe[0]);
                            close(outputpipe[1]);

                            //Debugging ended here
                            write(inputpipe[1], servSock[n].first.get_request().c_str(), servSock[n].first.get_request().size());
                            close(inputpipe[1]);
                            rsp.set_status(200);
                            char buf[1024];
                            ssize_t bytes;
                            std::string cgi_response;
                            while ((bytes = read(outputpipe[0], buf, sizeof(buf))) > 0)
                            {
                                cgi_response.append(buf, bytes);
                                //handle CGI script, senf it as HTTP response
                            }
                            close(outputpipe[0]);
                            std::string httpResponse;
                            //httpResponse += "Content-Type: text/html\r\n";
                            //httpResponse += "Content-Length: " + std::to_string(cgiOutput.size()) + "\r\n";
                            //httpResponse += "\r\n";
                            httpResponse += cgi_response;
                            ///+/ For demonstration, you can send the HTTP response to the client or display it
                            std::cerr << "HTTPS RESPONSE ====" << std::endl;
                            std::cerr << httpResponse << std::endl;
                            std::cerr << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
                            rsp + httpResponse;
                            buildResponse(n, rsp, rqst);
                            
                            std::cerr << rsp.get_response() << std::endl;
                        }

                        }else{
                            file = rqst.get_file();
                            rsp.set_status(200);
                        }
                    }
                }
            }
        }
        else
        {
            // if path doesnt mathc any file file = root + file; (root from serv or location)
            std::string path = rqst.get_file();
            if (servSock[n].second.getLocation("/") != NULL && servSock[n].second.getIndex().empty())
            { // file = locationroot + file requested
                path = (*servSock[n].second.getLocation("/")).getLocationRoot() + rqst.get_file();
                if (!access(path.c_str(), F_OK))
                {
                    std::cout << "\\\\\\\\\\\\\\\\";
                    file = path;
                    rsp.set_status(200);
                }
                else
                    rsp.set_status(404);
            }
            else if (servSock[n].second.getLocation("/") == NULL && !servSock[n].second.getIndex().empty())
            { // file = servRoot + file requested;
                path = servSock[n].second.getRoot() + rqst.get_file();
                if (!access(path.c_str(), F_OK))
                {
                    file = path;
                    rsp.set_status(200);
                }
                else
                {
                    rsp.set_status(404);
                }
            }
        }
        if (!autIndexed)
            buildResponse(n, rsp, rqst);
        sendResponse(n, rsp);
        servSock[n].first.close_sock();
        servSock.erase(servSock.begin() + n);
    }
    else
    {
        Handler handler(rqst, (*this), n);
        int status = handler.handleMethod();
        std::string response = "";
        if(status == 201){
            response = "HTTP/1.1 201 Created\r\nContent-Type: text/html\r\nContent-Length: 25\r\n\r\n<h1>201 file created</h1>";
        }else if (status == 204){
            response = "HTTP/1.1 204 No Content\r\n\r\n";
        }else if (status == 404){
            response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 27\r\n\r\n<h1>404 file not found</h1>";
        }else {
            response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length: 31\r\n\r\n<h1>405 Method not allowed</h1>";
        }
        send(servSock[n].first.get_socket(), response.c_str(), response.size(), 0);

    }
}

void ServSock::remove(int i)
{
    servSock.erase(servSock.begin() + i);
}

int ServSock::size()
{
    return servSock.size();
}

void ServSock::addPair(std::pair<Socket, Server> ss)
{
    servSock.push_back(ss);
}
std::pair<Socket, Server> &ServSock::operator[](int i)
{
    return servSock[i];
}

ServSock::~ServSock()
{
}
