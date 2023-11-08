#ifndef HTTPDESERIALIZER_HPP
#define HTTPDESERIALIZER_HPP
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "Request.hpp"
#include <iostream>

class HttpDeserializer
{
private:
    // std::map<std::string, std::string> headers;
    std::string body;
    Request requestLine;

public:
    bool Done;

public:
    HttpDeserializer(/* args */);
    void Deserialize(std::string request);
    void ParseHeaders(std::string headers_string);
    ~HttpDeserializer();
};

#endif