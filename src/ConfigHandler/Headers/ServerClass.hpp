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
    void Validate_Values();

    // Server Specific
public:
    std::string GetHostName();
    std::string GetPort();
    LocationClass *GetLocation(std::string &path);
    std::vector<std::string> &GetServerNames();

    // Shared  (Server | Location)
public:
    std::string GetErrorPagePath(int ErrorCode);
    std::string GetRoot(std::string Url);
    std::pair<int, std::string> GetRedirection(std::string &path);
    std::vector<std::string> GetIndex();
    bool GetAutoindex();
    bool GetUpload();
    std::string GetUpload_stor();
    std::vector<std::string> &GetCgi();
    std::string GetCgi_path();
    std::vector<std::string> GetAllowed();

public:
    void SetRequestPath(std::string &path);

private:
    std::string RawData;
    std::string port, host, root;
    std::vector<std::string> server_names;
    std::vector<LocationClass> locations;
    LocationClass *location;
    bool SSlOn;
    std::string SSlCertificate;
    std::string SSlcertificatekey;

    // Shared attr (Server | Location)
private:
    std::vector<std::string> index;
    std::pair<int, std::string> redirection;
    std::map<int, std::string> error_page;
    std::string autoindex;
    std::string upload;
    std::string upload_stor;
    std::vector<std::string> cgi;
    std::string cgi_path;
};
#endif
