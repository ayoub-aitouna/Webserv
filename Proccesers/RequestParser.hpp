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

class RequestParser
{

public:
    RequestParser();
    Medium *Parse(std::string data);
    void ParseHeaders(std::string data);
    int FillBody(std::string &data);
    void ParseBody();
    void GetResourceFilePath();
    void PrintfFullRequest();

private:
    std::map<std::string, std::string> Content_Types;
    Medium medium;
    std::string buffer;
    bool BodyReady;
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