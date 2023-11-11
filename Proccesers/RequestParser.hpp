#ifndef RequestParser_HPP
#define RequestParser_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "../Lib/Lstring.hpp"
#include "Medium/Medium.hpp"
#include "ResponseBuilder.hpp"
// #include <iostream>

enum
{
    Lenght,
    Chunked,
    Connection,
    NON
};

enum
{
    Get,
    Post,
    Delete,
    NON
};

class RequestParser
{
    typedef std::map<std::string, std::string>::iterator HeadersIterator;

public:
    RequestParser();
    Medium *Parse(std::string data);
    Medium *ParseBody(std::string data);
    void ParseHeaders(std::string data);
    void GetResourceFilePath();

private:
    std::map<std::string, std::string> Headers;
    std::map<std::string, std::string> Content_Types;
    ResourceFile File;
    std::string Url;
    std::string body;
    std::string tmp;
    int Method;
    int Remaining;
    int Encoding;

public:
    std::string GetHeaderAttr(std::string);

public:
    class HTTPError : public std::exception
    {
    public:
        int statusCode;
        HTTPError(int statusCode) : statusCode(statusCode) {}

    public:
        const char *what() const throw()
        {
            return "HTTPError";
        }
    };
};
#endif