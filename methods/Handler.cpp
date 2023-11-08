
#include "./Handler.hpp"
#include "../Includes/Request.hpp"

Handler::Handler(Request request){
    this->request = request;
}

// void Handler::handleMethod(){
//     if(this->request.get_method() == "POST"){
//         std::cout << "POST METHOD !" << std::endl;
//         std::string content = this->request.get_body();
//         std::cout << "TEXT START" << std::endl;
//         std::cout << "request content : " << content << std::endl;
//         // upload temp
//         std::string filepath = "/tmp/uploads/" + this->request.get_file();
//         std::ofstream outfile(filepath.c_str());
//         outfile << content << std::endl;
//         outfile.close();
//                 std::cout << "TEXT DONE" << std::endl;
//     }else if(this->request.get_method() == "DELETE"){
//         std::cout << "DELETE METHOD !" << std::endl;
//     }
// }