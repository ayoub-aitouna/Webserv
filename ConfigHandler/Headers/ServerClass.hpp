#ifndef SERVERCLASS
#define SERVERCLASS

#include "LocationClass.hpp"
#include <string>

class ServerClass
{

public:
    ServerClass(/* args */);
    ~ServerClass();

private:
    std::string port, host, root;
    std::vector<std::string> server_name;
    // std::string host;
    std::vector<std::pair<int, std::string> > error_page;
    LocationClass location;
};
#endif
