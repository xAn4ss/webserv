#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "../Includes/Request.hpp"
#include "../Includes/ServSock.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unistd.h>
//
#include <iostream>
#include <cstdio>
#include <cstring>


class Handler {

    Request request;
    int m;
    std::string _uploadPath;
    ServSock servSock;
    public:
        Handler(Request request, ServSock s, int n);
        int handleMethod();
};

Handler::Handler(Request request, ServSock s, int n){
    this->request = request;
    this->_uploadPath = (s)[n].second.getUploadPath();
    this->servSock = s;
    this->m = n;
}

void deleteFileOrDirectory(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        if (S_ISREG(st.st_mode)) { // Check if it's a regular file
            if (remove(path.c_str()) == 0) {
                return;
            }
        } else if (S_ISDIR(st.st_mode)) { // Check if it's a directory
            if (rmdir(path.c_str()) == 0) {
                return;
            }
        }
    }

    std::cerr << "Error: Unable to delete " << path << std::endl;
}

int Handler::handleMethod(){
    // POST METHOD
    if(this->request.get_method() == "POST"){
        ServLocation* tmp = servSock[m].second.getLocation(request.get_file().substr(1));
        if (tmp->getLocationIsCgi() == true){
            std::string cgiPath = tmp->getLocationCgiPath();
            std::string content = this->request.get_body();

            //EXECUTE CGI SCRIPT
            int inputPipe[2];
            int outputPipe[2];

            if (pipe(inputPipe) != 0 || pipe(outputPipe) != 0)
            {
                std::cerr << "Error: pipe() failed!" << std::endl;
            }

            pid_t pid = fork();

            if (pid == -1){
                std::cerr << "Error: fork() failed!" << std::endl;    
            }
            else if (pid == 0)
            {
                //child process
                Response rsp;
                close(inputPipe[1]);
                close(outputPipe[0]);

                dup2(inputPipe[0], STDIN_FILENO);
                dup2(outputPipe[1], STDOUT_FILENO);

                char wor[256];
                getcwd(wor, 256);
                std::string cgi_path = wor;
                std::string script_path = wor;
                cgi_path += "/";
                cgi_path += tmp->getLocationCgiFile();
                script_path += "/"; 
                script_path += tmp->getLocationCgiPath();
                char *argv[] = {(char *)cgi_path.c_str(), (char *)script_path.c_str(), NULL};
                std::map<std::string, std::string> env = servSock.parseRequestHeader(request, m, cgi_path);

                char **envp = convertToEnvp(env);
                if (execve(argv[0], argv, envp))
                {
                    std::cerr << "Error: execve() failed!" << std::endl;
                }
                else{
                    close(inputPipe[0]);
                    close(outputPipe[1]);

                    write(inputPipe[1], content.c_str(), content.length());
                    close(inputPipe[1]);

                    rsp.set_status(200);
                    char buffer[128];
                    ssize_t bytesRead;
                    std::string output;
                    while ((bytesRead = read(outputPipe[0], buffer, 128)) > 0)
                    {
                        output.append(buffer, bytesRead);
                    }
                    close(outputPipe[0]);
                    std::string httpResponse;
                    httpResponse += output;
                    rsp + httpResponse;
                    servSock.buildResponse(m, rsp, request);
                    //servSock.sendResponse(m, rsp);
                }

    }
        }else{
        std::cout << "POST METHOD !" << std::endl;
        std::string content = this->request.get_body();
        //std::cout << "content is : " << content << std::endl;
        std::string filepath = this->_uploadPath + this->request.get_file();
        //std::cerr << "@@@@@ REQUEST : " << this->request.get_body() << std::endl;
        std::ofstream outfile(filepath.c_str());
        outfile << content << std::endl;
        outfile.close();
        return 201;
        }
    }

    //DELETE METHOD
    else if(this->request.get_method() == "DELETE")
    {

        std::string location = request.get_file(); // file to delete 
        std::string directoryPath = _uploadPath; // The directory path
                // Split the location into the directory and filename
        std::size_t lastSlash = location.rfind('/');
        std::string filename;
        if (lastSlash != std::string::npos){
            filename = location.substr(lastSlash + 1);
        }else {
            filename = location;
        }

        // Create the full path by concatenating the directory and filename
        std::string fullPath = directoryPath + filename;


        std::cout <<" path is :"<<fullPath << std::endl;




        if (location[0] == '/' && location.length() > 1)
            location.erase(0, 1);
        if (location.length() == 1 && location[0] == '/')
            return 403 ;
        if (access(fullPath.c_str(), F_OK) == -1) {
            return 404;
        }
        deleteFileOrDirectory(fullPath);  
    }
    return 204;
}


#endif