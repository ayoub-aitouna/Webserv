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
    HttpClass();
    void SetRawData(std::string &RawData);
    void Parse();
    void DisplayValues(bool Show);
    std::string GetContentType(std::string FileExtention);
    std::string GetFileExtention(std::string ContentType);
    std::string GetGlobalErrorPagePath(int ErrorCode);
    unsigned long long GetMaxBodySize();
    std::vector<ServerClass> &GetServers();
    ServerClass *GetServersByHost(std::string host);
    ~HttpClass();

private:
    std::string RawData;
    TypesClass types;
    std::string default_type;
    unsigned long long client_max_body_size;
    std::map<int, std::string> error_page;
    std::vector<ServerClass> servers;
};

template <typename T>
bool Containes(std::vector<T> Vec, T item)
{
    for (typename std::vector<T>::iterator i = Vec.begin(); i != Vec.end(); i++)
    {
        if (*i == item)
            return (true);
    }
    return (false);
}

#endif