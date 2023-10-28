
#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <iostream>
#include <fstream>

class Handler {

    Request request;

    public:
        Handler(Request request);
        void handleMethod();
};

Handler::Handler(Request request){
    this->request = request;
}

void Handler::handleMethod(){
    if(this->request.get_method() == "POST"){
        std::cout << "POST METHOD !" << std::endl;
        std::string content = this->request.get_body();
        std::cout << "request content : " << content << std::endl;
        // upload temp
        std::string filepath = "/tmp/uploads/" + this->request.get_file();
        std::ofstream outfile(filepath);
        outfile << content << std::endl;
        outfile.close();
    }else if(this->request.get_method() == "DELETE"){
        std::cout << "POST METHOD !" << std::endl;
    }
}

#endif