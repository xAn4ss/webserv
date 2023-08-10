#ifndef REQUEST_HPP
#include <iostream>
#include <string>
#include <sstream>
#include <stack>

class Request
{
private:
    std::string method;
    std::string file;
    std::string http_v;
public:
    Request();
    ~Request();
    int parse(std::string req);
    std::string get_method();
    std::string get_file();
    std::string get_http_v();
    void setMethod(std::string);
    void setFile(std::string);
    void setHttpV(std::string);
};
void Request::setMethod(std::string s){
    method = s;
}

void Request::setFile(std::string s){
    file = s;
}

void Request::setHttpV(std::string s){
    http_v = s;
}


std::string Request::get_method(){
    return this->method;
}

std::string Request::get_file(){
    return this->file;
}

std::string Request::get_http_v(){
    return this->http_v;
}
int Request::parse(std::string req){
    std::istringstream r(req);
    r >> method;
    r >> file;
    r >> http_v;
    return 0;
}

Request::Request(/* args */)
{
}

Request::~Request()
{
}


#endif