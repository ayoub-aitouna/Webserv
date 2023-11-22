#ifndef EVENTCLASS
#define EVENTCLASS

#include <string>
#include <vector>
#include "ServerClass.hpp"
#include "TypesClass.hpp"
class HttpClass
{

public:
    HttpClass(/* args */);
    void Parse(std::string &block);
    ~HttpClass();

private:
    TypesClass types;
    std::string default_type;
    unsigned long client_max_body_size;
    std::vector<std::pair<int, std::string> > error_page;
    ServerClass server;
};

#endif