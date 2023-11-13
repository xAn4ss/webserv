#include "cgi_handler.hpp"
#include "../Includes/ServSock.hpp"   
#include "../Includes/Request.hpp"

std::map<std::string, std::string> ServSock::parseRequestHeader(Request rqt, int n, std::string cgi_path) {
    std::map<std::string, std::string> headerFields;

    std::cerr << "//////////////////POST METHOD !//////////////" << std::endl;
    const std::string& request = rqt.get_header();
    std::cerr << "n=" << n << std::endl;
    std::cerr << "request: " << request << std::endl;
    std::cerr << "@@@@@@@@@@@@@@@@@@@" << std::endl;
    std::istringstream ss(request);
    std::string line;
    std::map<std::string, std::string> environmentVariables;

    std::cerr << "cgi_path=" + cgi_path << std::endl;
    while (std::getline(ss, line)) {
    std::cerr << "/////////" << line << std::endl;
        std::size_t colonPos = line.find(':');

        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 2);

            // Trim any leading/trailing spaces from the value
            value.erase(0, value.find_first_not_of(' '));
            value.erase(value.find_last_not_of(' ') + 1);

            // Insert the key-value pair into the map
            headerFields[key] = value;
        }
    }
    //for (const auto& pair : headerFields) {
    //    std::cerr << pair.first << " $$$ " << pair.second << std::endl;
    //}
    std::cerr << "///////////////////////////////////////" << std::endl;
    // Construct environment variables similar to Response::initEnv
    std::vector<std::string> vecEnv;
    
    if (headerFields.find("Accept") != headerFields.end())
        vecEnv.push_back(std::string("HTTP_ACCEPT=") + headerFields["Accept"]);
    // Add more environment variables similarly for other fields as in the initEnv function
    vecEnv.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
    vecEnv.push_back(std::string("SCRIPT_NAME=") + (*servSock[n].second.getLocation(rqt.get_file())).getLocationCgiPath());
    vecEnv.push_back(std::string("SCRIPT_FILENAME=") + (*servSock[n].second.getLocation(rqt.get_file())).getLocationCgiFile());
    vecEnv.push_back(std::string("PATH_TRANSLATED=") + (*servSock[n].second.getLocation(rqt.get_file())).getLocationCgiFile());
    vecEnv.push_back(std::string("PATH_INFO=") + (*servSock[n].second.getLocation(rqt.get_file())).getLocationCgiFile());
    if (headerFields.find("Content-Type") != headerFields.end())
        vecEnv.push_back(std::string("CONTENT-TYPE=") + headerFields["Content-Type"]);
    if (headerFields.find("Content-Length") != headerFields.end())
        vecEnv.push_back(std::string("CONTENT_LENGTH=") + headerFields["content-length"]);
    vecEnv.push_back(std::string("REDIRECT_STATUS=200"));
    vecEnv.push_back(std::string("SERVER_NAME=") + headerFields["Host"]);
    vecEnv.push_back(std::string("SERVER_PORT=") + headerFields["Host"].substr(headerFields["Host"].find(":") + 1));

    // Mapping other fields to environment variables
    std::cerr << "server port =" << headerFields["Host"].substr(headerFields["Host"].find("localhost:") + 1) << std::endl;
    std::cerr << "server name =" << headerFields["Host"] << std::endl;
    std::cerr << "script file name =" << (*servSock[n].second.getLocation(rqt.get_file())).getLocationCgiFile() << std::endl;
    std::cerr << "script name =" << (*servSock[n].second.getLocation(rqt.get_file())).getLocationCgiPath() << std::endl;
    
    // Example: Add REQUEST_METHOD
    vecEnv.push_back(std::string("REQUEST_METHOD=") + rqt.get_method()); // Assuming it's a GET request
    //if (headerFields.find("POST") != headerFields.end())
    //    vecEnv.push_back(std::string("QUERY_STRING="));

    // Example: Map Host to SERVER_NAME
    if (headerFields.find("Host") != headerFields.end())
        vecEnv.push_back(std::string("SERVER_NAME=") + headerFields["Host"]);

    // Constructing the map of environment variables
    for (std::vector<std::string>::const_iterator it = vecEnv.begin(); it != vecEnv.end(); ++it) {
        const std::string& envVar = *it;
        size_t pos = envVar.find('=');
        
        if (pos != std::string::npos) {
            std::string key = envVar.substr(0, pos);
            std::string value = envVar.substr(pos + 1);
            environmentVariables[key] = value;
        }
    }
    std::cerr << "///////////***************///////////////" << std::endl;
    std::cerr << "###################Environment Variables:################" << std::endl;
    //for (const auto& pair : environmentVariables) {
    //    std::cerr << pair.first << " = " << pair.second << std::endl;
    //}

    return environmentVariables;
}


char** convertToEnvp(const std::map<std::string, std::string>& requestMap) {
    char** envp = new char*[requestMap.size() + 1]; // +1 for NULL terminator
    int i = 0;

    for (std::map<std::string, std::string>::const_iterator it = requestMap.begin(); it != requestMap.end(); ++it) {
        std::string envVar = it->first + "=" + it->second;
        envp[i] = new char[envVar.size() + 1]; // +1 for null-terminator
        std::strcpy(envp[i], envVar.c_str());
        ++i;
    }

    envp[i] = NULL; // Terminate the envp array with a NULL pointer
    return envp;
}
