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

public:
    std::string GetErrorPagePath(int ErrorCode);
    std::string GetHostName();
    std::string GetPort();
    std::string GetRoot(std::string &Url);
    std::vector<std::string> &GetServerNames();
    std::pair<int, std::string> GetRedirection(std::string &path);
    std::vector<std::string> GetIndex();
    LocationClass *GetLocation(std::string &path);

public:
    void SetRequestPath(std::string &path);

private:
    std::string RawData;
    std::string port, host, root;
    std::vector<std::string> index;
    std::vector<std::string> server_name;
    std::pair<int, std::string> redirection;
    std::map<int, std::string> error_page;
    std::vector<LocationClass> locations;
    LocationClass *location;
};
#endif
