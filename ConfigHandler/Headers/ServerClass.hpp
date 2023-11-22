#ifndef SERVERCLASS
#define SERVERCLASS

#include "LocationClass.hpp"
#include <string>
#include "Preprocessor.hpp"

class ServerClass
{

public:
    ServerClass(std::string &RawData);
    void Parse();
    ~ServerClass();

private:
    std::string RawData;
    std::string port, host, root;
    std::vector<std::string> server_name;
    std::pair<int, std::string> redirection;
    // std::string host;
    std::vector<std::pair<int, std::string> > error_page;
    std::vector<LocationClass> locations;
};
#endif
