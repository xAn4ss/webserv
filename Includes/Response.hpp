#ifndef RESPONSE_HPP
#include <fstream>
#include <iostream>
#include "Request.hpp"

class Response
{
private:
    int status;
    std::string data;
public:
    Response();
    ~Response();
    int get_status();
    void set_status(int code);
    std::string get_response();
    void set_response(std::string data);
    void BuildResponse(Request rqst, std::string file);
    void setResponseContentType(std::string file);    
    void operator+(std::string);
};

#endif