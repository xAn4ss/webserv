#include "../Includes/Request.hpp"

std::string Request::get_body(){
    return this->body;
}

void Request::setMethod(std::string s){
    method = s;
}

void Request::setFile(std::string s){
    file = s;
}

void Request::setHttpV(std::string s){
    http_v = s;
}

void Request::setBody(std::string s){
    body = s;
}

std::string Request::get_method(){
    return this->method;
}

std::string Request::get_file(){
    return this->file;
}

void Request::setHeader(std::string s){
    header = s;
}
std::string Request::get_header(){
    return this->header;
}

std::string Request::get_http_v(){
    return this->http_v;
}
int Request::parse(std::string req){
    std::string body = "";
    std::istringstream r(req);
    r >> method;
    r >> file;
    r >> http_v;
    header = req.substr(0, req.find("\r\n\r\n"));
    // parse request body 
    std::size_t bodyIndex = req.find("\r\n\r\n");
    if (bodyIndex != std::string::npos) {
        bodyIndex += 4; 
        body = req.substr(bodyIndex);
    } 
    this->body = body;
    // std::cerr << "HEADER:\n" <<  this->header << std::endl;
    return 0;
}

Request::Request(/* args */)
{
}

Request::~Request()
{
}