#ifndef Request_HPP
#define Request_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "../Lib/Lstring.hpp"
#include "../Lib/ResourceFile.hpp"
#include "Response.hpp"
// #include <iostream>

class Request
{
    typedef std::map<std::string, std::string>::iterator HeadersIterator;

public:
    Request();
    ResourceFile Parse(std::string data);
    void ParseHeaders(std::string data);
    void GetResourceFilePath();

private:
    std::map<std::string, std::string> Headers;
    std::map<std::string, std::string> Content_Types;
    ResourceFile file;
    std::string Method;
    std::string Url;
    std::string body;

public:
    const std::string &GetMethod() const;
    const std::string &GetUrl() const;
    std::string GetHeaderAttr(std::string);
    std::string &GetBody();
    ResourceFile &GetResourceFile();
};
#endif