#ifndef HTTPCLASS_HPP
#define HTTPCLASS_HPP

#include <string>
#include <vector>
#include "ServerClass.hpp"
#include "TypesClass.hpp"
#include "Preprocessor.hpp"

class HttpClass
{

public:
    HttpClass(std::string &RawData);
    void Parse();
    ~HttpClass();

private:
    std::string &RawData;
    TypesClass types;
    std::string default_type;
    unsigned long client_max_body_size;
    std::vector<std::pair<int, std::string> > error_page;
    std::vector<ServerClass> servers;
};

#endif