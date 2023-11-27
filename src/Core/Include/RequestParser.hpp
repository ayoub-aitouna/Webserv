#ifndef RequestParser_HPP
#define RequestParser_HPP

#include "Core.hpp"
#include "AutoIndex.hpp"
#include "../RequestHandlers/Include/Request.hpp"
#include "../RequestHandlers/Include/PostRequest.hpp"
#include "../RequestHandlers/Include/GetRequest.hpp"
#include "../RequestHandlers/Include/DeleteRequest.hpp"

enum ConnectionState
{
    READ_HEADERS,
    PROCESS_REQUEST,
};

class RequestParser
{

public:
    RequestParser();
    bool Parse(std::string data);
    void ParseHeaders(std::string data);
    void ParseUrl(std::string &Url);
    void RequestHandlersFactory(std::string &Method);
    DataPool &GetDataPool();
    Request *GetRequestHandler();
    ~RequestParser();

private:
    Request *RequestHandler;
    std::string buffer;
    DataPool dataPool;
    ConnectionState state;
};

std::string CreateRow(int type, std::string Name, u_int64_t size, std::string permissions);
#endif