#ifndef RequestParser_HPP
#define RequestParser_HPP

#include "Proccesers.hpp"
#define NOPOS std::string::npos
class RequestParser
{

public:
    RequestParser();
    bool Parse(std::string data);
    void ParseHeaders(std::string data);
    int FillBody(std::string &data);
    void ParseBody();
    void GetResourceFilePath();
    void PrintfFullRequest();
    std::string GetHeaderAttr(std::string name);
    WBSRVFILE SaveAsFile(std::string &part);

private:
    std::map<std::string, std::string> Content_Types;
    std::map<std::string, std::string> Reverse_Content_Types;
    std::string buffer;
    bool BodyReady;
    int Remaining;
    int Encoding;

public:
    DataPool dataPool;

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