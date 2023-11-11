#include "../Includes/Response.hpp"

void Response::operator+(std::string s){
    data += s;
}


std::string Response::get_response(){
    return data;
}
void Response::set_response(std::string data){
    this->data = data;
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