#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <string>
#include <sstream>
#include <iostream>
#include <stack>

class Request
{
private:
    std::string method;
    std::string file;
    std::string http_v;
    std::string body;
    std::string header;
public:
    Request();
    ~Request();
    int parse(std::string req);
    std::string get_method();
    std::string get_file();
    std::string get_http_v();
    std::string get_body();
    std::string get_header();
    void setBody(std::string req);
    void setMethod(std::string);
    void setFile(std::string);
    void setHttpV(std::string);
    void setHeader(std::string);
};

#endif