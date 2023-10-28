#include "Includes/Response.hpp"

void Response::operator+(std::string s){
    data += s;
}


std::string Response::get_request(){
    return data;
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