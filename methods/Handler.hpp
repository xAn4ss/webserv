
#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "../Includes/Request.hpp"
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

    public:
        Handler(Request request);
        int handleMethod();
};

Handler::Handler(Request request){
    this->request = request;
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
        std::cout << "POST METHOD !" << std::endl;
        std::string content = this->request.get_body();
        std::string filepath = "uploads/" + this->request.get_file();
        std::ofstream outfile(filepath.c_str());
        outfile << content << std::endl;
        outfile.close();
        return 201;
    }

    //DELETE METHOD
    else if(this->request.get_method() == "DELETE")
    {

        std::string location = request.get_file(); // file to delete 
        std::string directoryPath = "uploads/"; // The directory path
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