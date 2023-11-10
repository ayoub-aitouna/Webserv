#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <string>
#include "../Response.hpp"

class File
{
public:
    int File;
    std::string FileType;
};

class RequestParser
{
public:
    RequestParser();
    virtual RequestParser *Create(std::string Method, std::string Url, std::string Headers) = 0;
    virtual void Parse(std::string data) = 0;
    void UriDecode();
    File GetFilePath();
    ~RequestParser();

protected:
    std::string method;
    std::string url;
    std::map<std::string, std::string> Content_Types;
};
#endif