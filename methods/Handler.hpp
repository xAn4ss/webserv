
#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "../Includes/Request.hpp"
#include <iostream>
#include <fstream>

class Handler {

    Request request;

    public:
        Handler(Request request);
        int handleMethod();
};

Handler::Handler(Request request){
    this->request = request;
}

int Handler::handleMethod(){
    if(this->request.get_method() == "POST"){
        std::cout << "POST METHOD !" << std::endl;
        std::string content = this->request.get_body();
        std::cout << "TEXT START" << std::endl;
        //  std::cout << "request content : " << content << std::endl;
        // upload temp
        std::string filepath = "/tmp/uploads/" + this->request.get_file();
        std::ofstream outfile(filepath.c_str());
        outfile << content << std::endl;
        outfile.close();
        std::cout << "TEXT DONE" << std::endl;
        return 201;
    }else if(this->request.get_method() == "DELETE"){
        std::cout << "DELETE METHOD !" << std::endl;
        return 204;
    }
    return 404;
}
// void Handler::handleMethod(){
//     if(this->request.get_method() == "POST"){
//         std::cout << "POST METHOD !" << std::endl;
//         std::string content = this->request.get_body();
//         std::cout << "request content : " << content << std::endl;
//         // upload temp
//         std::string filepath = "/tmp/uploads/" + this->request.get_file();
//         std::ofstream outfile(filepath.c_str());
//         outfile << content << std::endl;
//         outfile.close();
//     }else if(this->request.get_method() == "DELETE"){
//         std::cout << "POST METHOD !" << std::endl;
//     }
// }

#endif