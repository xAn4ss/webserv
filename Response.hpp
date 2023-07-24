#ifndef RESPONSE_HPP
#include <iostream>

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
};

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