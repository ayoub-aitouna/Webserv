#ifndef RequestParser_HPP
#define RequestParser_HPP

#include "Proccesers.hpp"
class RequestParser
{

public:
    RequestParser();
    bool Parse(std::string data);
    void ParseHeaders(std::string data);
    void ParseUrl(std::string &Url);
    void ParseBody();
    void GetResourceFilePath();
    void PrintfFullRequest();
    int FillBody(std::string &data);
    void AutoIndex(std::string &dirPath);
    std::string GetHeaderAttr(std::string name);
    WBSRVFILE SaveMultiPartFile(std::string &part);
    WBSRVFILE SaveBodyAsFile();

private:
    std::map<std::string, std::string> Content_Types;
    std::map<std::string, std::string> Reverse_Content_Types;
    bool BodyReady;
    long Remaining;
    int Encoding;
    int ChunkState;

public:
    std::string buffer;
    DataPool dataPool;
    long datasize;

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