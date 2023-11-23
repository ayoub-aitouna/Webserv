#ifndef SERVERCLASS
#define SERVERCLASS

#include "LocationClass.hpp"
#include <string>
#include "Preprocessor.hpp"

class ServerClass
{

public:
    ServerClass();
    ServerClass(std::string &RawData);
    ServerClass(const ServerClass &lhs);
    void SetData(std::string &RawData);
    ServerClass &operator=(const ServerClass &lhs);
    ~ServerClass();

public:
    void Parse();
    void DisplayValues(bool Show);
    std::string GetErrorPagePath(int ErrorCode);
    std::string GetHostName();
    std::string GetPort();
    std::string GetRoot();
    std::vector<std::string> &GetServerNames();
    std::pair<int, std::string> GetRedirection();
    LocationClass *GetLocation(std::string &path);

private:
    std::string RawData;
    std::string port, host, root;
    std::vector<std::string> server_name;
    std::pair<int, std::string> redirection;
    std::map<int, std::string> error_page;
    std::vector<LocationClass> locations;
};
#endif
