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
    OTHER
};

class RequestParser
{
    typedef std::map<std::string, std::string>::iterator HeadersIterator;

public:
    RequestParser();
    Medium *Parse(std::string data);
    void ParseHeaders(std::string data);
    int FillBody(std::string &data);
    void ParseBody();
    void GetResourceFilePath();
    std::string GetHeaderAttr(std::string);
    void PrintfFullRequest();

private:
    std::map<std::string, std::string> Headers;
    std::map<std::string, std::string> Content_Types;
    std::string buffer;
    ResourceFile File;
    std::string Url;
    std::string body;

    bool BodyReady;
    int Method;
    int Remaining;
    int Encoding;

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