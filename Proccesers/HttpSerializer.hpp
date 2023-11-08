#ifndef HTTPSERIALIZER_HPP
#define HTTPSERIALIZER_HPP
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "Request.hpp"
#include <iostream>

class HttpSerializer
{
private:
    Request requestLine;

public:
    bool DoneDeserializing;

public:
    HttpSerializer(/* args */);
    void Deserialize(std::string request);
    std::string Serialize();
    void ParseHeaders(std::string headers_string);
    ~HttpSerializer();
};

#endif