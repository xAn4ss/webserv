#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <iostream>

char** convertToEnvp(const std::map<std::string, std::string>& requestMap);

#endif