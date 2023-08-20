#ifndef RESPONSE_HPP
#include <iostream>
#include "Request.hpp"
class Response
{
private:
    int status;
    std::string body;
public:
    Response();
    ~Response();
    int get_status();
    void set_status(int code);
    std::string get_request();
    void BuildResponse(Request rqst, std::string file);
    
};

void Response::BuildResponse(Request rqst ,std::string file){
            body += rqst.get_http_v() + " ";
            body += std::to_string(status);
            if (status == 200)
                body += " OK\n";
            else if (status == 301)
                body += " Moved Permantely\nLocation: " + file +"\n";
            body += " Content-Type: ";
            std::cout << file.length() - file.find(".html") << std::endl;
            if ((file.length() - file.find(".css")) == 4){
                body += "text/css\n\n";
            }else if((file.length() - file.find(".html")) == 5){
                body += "text/html\n\n";
            }
            std::ifstream f(file);
            std::string tm;
            while (getline(f, tm)){
                body += tm + '\n';
            }
            std::cout << "==> "<< body << std::endl;
}

std::string Response::get_request(){
    return body;
}

void Response::set_status(int code){
        this->status = code;
}

int Response::get_status(){
    return this->status;
}

Response::Response()
{
}

Response::~Response()
{
}

#endif