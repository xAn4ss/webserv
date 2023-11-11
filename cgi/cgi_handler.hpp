#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <iostream>

std::map<std::string, std::string> parseRequestHeader(const std::string& request);
char** convertToEnvp(const std::map<std::string, std::string>& requestMap);

#endif