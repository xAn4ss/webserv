#ifndef RESPONSE_HPP
#include <fstream>
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
    void setResponseContentType(std::string file);    
    void operator+(std::string);
};

void Response::operator+(std::string s){
    body += s;
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