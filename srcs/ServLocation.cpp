#include "../Includes/ServLocation.hpp"

ServLocation::ServLocation(){
    _index = "";
    _isAutoIndex = false;
    _isRedirected = false;
    _methods.insert(std::make_pair("GET", 1));
    _methods.insert(std::make_pair("POST", 1));
    _methods.insert(std::make_pair("DELETE", 1));

}




int ServLocation::setLocationMethods(std::string *method, int &size){
    if (size < 2){
        std::cout << "Error in method directive" << std::endl;
        return 1;
    }
    std::map<std::string, int>::iterator it = _methods.begin();
    while (it != _methods.end()){
            int f = 0;
        for (int i = 1; i < size; i++){
            if (method[i].compare("GET") && method[i].compare("POST") 
             && method[i].compare("DELETE")){
                std::cout << "Error in methods syntax." << std::endl;
                return 1;
            }
            // std::cout << it->first<< "("<< method[i] << ")";
            if (it->first == method[i])
            {
                f = 1;
                it->second++;
            }
            else if (!f && it->first != method[i])
            {
                it->second = 0;

            }
            // std::cout << it->second << std::endl;
        }
        it++;
    }
    // std::cout << "Methods are :"<< std::endl;
    // std::cout << "GET : "<< _methods["GET"]<< std::endl;
    // std::cout << "POST : "<< _methods["POST"]<< std::endl;
    // std::cout << "DELETE : "<< _methods["DELETE"]<< std::endl;
    return 0;
}

ServLocation::~ServLocation()
{
}


std::map<std::string, int> ServLocation::getLocationMethods(){
    return _methods;
}


bool ServLocation::getLocationAutoIndex()
{
    return _isAutoIndex;
}

std::string ServLocation::getLocationIndex(){
    if(_index.empty())
        return "";
    return _index;
}

std::string ServLocation::getLocationPath(){
    if (_locationPath.empty())
    {
        return "empty";
    }
    return _locationPath;
}
std::string ServLocation::getLocationRoot(){
    return _root;
}

std::string* splitIt(std::string s, int & size){
    int start = 0;
    int end = 0;
    std::vector<std::string> tmp;
    while (end != -1)
    {
        end = s.find_first_of(" \t", start);
        if (!s.substr(start, end - start).empty())
            tmp.push_back(s.substr(start, end - start));
        start = end + 1;
    }
    int x = -1;
    size = tmp.size();
    std::string *line = new std::string[size];
    while (++x < size)
        line[x] = tmp[x];
    return (line);
}

int ServLocation::setLocationIndex(std::string* s, int& size){
    if (size != 2)
    {
        std::cout << "missing index page" << std::endl;
        return 1;
    }
    std::string path = _root + s[1];
    if (access(path.c_str(), F_OK) == -1)
    {
        std::cout << "wrong server index path"<< std::endl;
        return 1;
    }
    _index = path;
    // maybe replace it with path
    return 0;
}

int ServLocation::setLocationRoot(std::string* s, int& size){
    if (size != 2)
    {
        std::cout << " error in Location \"" << _locationPath << "\" root directive." << std::endl;
        return 1;
    }
    if (!opendir(s[1].c_str()))
    {
        std::cout << "error in location root path." << std::endl;
        return 1;
    }
    _root = s[1];
    return 0;
}

int isAllDigit(std::string s){
    for( size_t i = 0; i < s.size(); i++){
        if (!isdigit(s[i]))
            return 1;
    }
    return 0;
}

int ServLocation::setLocationIsRedirect(std::string *s, int& size){
    if (size != 3){
        std::cout << "Error in redirection direcive" << std::endl;
        return 1;
    }
    if (!isAllDigit(s[1])){
        _Redir_code = s[1];
    }else{
        std::cout << "Error in redirection code " << std::endl;
        return 1;
    }
    _isRedirected = true;
    _redirected_to = s[2];
    return 0;
}

int ServLocation::setLocationAutoIndex(std::string *s, int& size){
    if (size != 2)
    {
        std::cout << "error in Location autoIndex directive." << std::endl;
        return 1;
    }
    if (!strncmp("on", s[1].c_str(), 2))
        _isAutoIndex = true;
    else if (!strncmp("off", s[1].c_str(), 3))
        _isAutoIndex = false;
    return 0;
}

int ServLocation::setLocationPath(std::string s)
{
    if (!opendir(s.c_str()))
    {
        std::cout << "error in location path" << std::endl;
        return 1;
    }
    _locationPath = s;
    return 0;
}

int ServLocation::setLocationCgiPath(std::string *s, int& size){
    if (size != 2){
        std::cout << "error in Location cgi path" << std::endl;
        return 1;
    }
    struct stat slatt;
    if (!stat(s[1].c_str(), &slatt) && S_ISDIR(slatt.st_mode))
        _cgi_path = s[1];
    else
    {
        std::cout << "error in Location cgi path (path inexistant)" << std::endl;
        return 1;
    }
    return 0;
}

int ServLocation::setLocationCgiExec(std::string *s, int& size){
    if (size < 2 || size > 3){
        std::cout << "error in location cgi execs" << std::endl;
        return 1;
    }
    for (int i = 1; i < size; i++)
    {
        if (s[i].compare("py") && s[i].compare("php"))
        {
            std::cout << "error in location cgi execs" << std::endl;
            return 1;
        }
        _cgi_execs.push_back(s[i]);
    }
    return 0;
}
bool ServLocation::getLocationIsRedirected(){
    return _isRedirected;
}

int ServLocation::getLocationRedirCode(){
    std::istringstream iss(_Redir_code);
    int code;
    iss >> code;
    return code;
}

std::string ServLocation::getLocationRedirPath(){
    return _redirected_to;
}
int ServLocation::processLocation(std::vector<std::string>::iterator begin,std::vector<std::string>::iterator end){
    std::vector<std::string> lct(begin, end);
    // std::cout << "-* "<<(*begin).substr((*begin).find_first_of(" \t")+1,
    // ((*begin).size()- (*begin).find_last_not_of("{ \t"))) << std::endl;

    int b = (*begin).find_first_of(" \t") + 1;
    int e = (*begin).find_last_not_of("{ \t") ;

    // std::cout << b <<" *-* " << e << " **" << (*begin).substr(b, e + 1 -b) <<".."<<std::endl;
    if (setLocationPath((*begin).substr(b, e + 1 -b)))
        return 1;
    int size;
    begin++;
    if (*((*begin).rbegin()) == '{')
        begin++;
    while (begin != end)
    {
        size = 0;
        if (!strncmp((*begin).c_str(), "root", 4)){
            if (setLocationRoot(splitIt(*begin, size), size))
                return 1;
        }else if (!strncmp((*begin).c_str(), "index", 5)){
            if (setLocationIndex(splitIt((*begin).c_str(), size), size))
                return 1;
        }else if (!strncmp((*begin).c_str(), "autoIndex", 9)){
            if (setLocationAutoIndex(splitIt((*begin).c_str(), size), size))
                return 1;
        }
        else if (!strncmp((*begin).c_str(), "method",6)){
            if (setLocationMethods(splitIt((*begin).c_str(), size), size))
                return 1;
        }else if (!strncmp((*begin).c_str(), "cgi_path", 8)){
            if (setLocationCgiPath(splitIt((*begin).c_str(), size), size))
                return 1;   
        }else if (!strncmp((*begin).c_str(), "cgi_exec", 8)){
            if (setLocationCgiExec(splitIt((*begin).c_str(), size), size))
                return 1;   
        }else if (!strncmp((*begin).c_str(), "return", 6)){
            if (setLocationIsRedirect(splitIt((*begin).c_str(), size), size))
                return 1;   
        }
        
        // std::cout << *begin << std::endl;
        begin++;
    }



    return 0;
}
