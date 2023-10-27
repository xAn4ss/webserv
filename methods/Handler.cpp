
#include "./Handler.hpp"

Handler::Handler(Request request){
    this->request = request;
}

void Handler::handleMethod(){
    if(this->request.get_method() == "POST"){
        std::cout << "POST METHOD !" << std::endl;
    }else if(this->request.get_method() == "DELETE"){
        std::cout << "POST METHOD !" << std::endl;
    }
}