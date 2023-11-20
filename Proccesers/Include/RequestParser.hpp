#ifndef RequestParser_HPP
#define RequestParser_HPP

#include "Proccesers.hpp"
#include "AutoIndex.hpp"
#include "../RequestHandlers/Include/Request.hpp"
#include "../RequestHandlers/Include/PostRequest.hpp"
#include "../RequestHandlers/Include/GetRequest.hpp"
#include "../RequestHandlers/Include/DeleteRequest.hpp"

class RequestParser
{

public:
    RequestParser();
    bool Parse(std::string data);
    void ParseHeaders(std::string data);
    void ParseUrl(std::string &Url);
    void RequestHandlersFactory(std::string &Method);
    Request *GetRequestHandler();
    ~RequestParser();

private:
    Request *RequestHandler;
    std::string buffer;

public:
    DataPool dataPool;
};

std::string CreateRow(int type, std::string Name, u_int64_t size, std::string permissions);
#endif