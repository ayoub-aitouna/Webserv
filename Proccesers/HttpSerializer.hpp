#ifndef HTTPSERIALIZER_HPP
#define HTTPSERIALIZER_HPP
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "Request.hpp"
#include "Response.hpp"
#include <iostream>
#include <sys/socket.h>

class HttpSerializer
{
private:
    Request request;
    Response *response;

public:
public:
    HttpSerializer(/* args */);
    void Deserialize(std::string request);
    ~HttpSerializer();

public:
    std::string Serialize();
    void ParseHeaders(std::string headers_string);
    Request &GetRequest();
    Response *GetResponse();
};

#endif