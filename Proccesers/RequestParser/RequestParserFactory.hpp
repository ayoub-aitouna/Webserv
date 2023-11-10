#ifndef REQUESTPARSERFACTORY_HPP
#define REQUESTPARSERFACTORY_HPP

#include <string>
#include <map>
#include "../Response.hpp"
#include "RequestParser.hpp"

typedef RequestParser *(RequestParser::*Create)(std::string, std::string, std::string);

class RequestParserFactory
{
public:
    RequestParserFactory();
    RequestParser *CreateParse(const std::string &data);

    template <typename T>
    void RegisterParser(const std::string &Method, Create Parser);
    
    ~RequestParserFactory();

private:
    std::map<std::string, Create> parserRegistry;
};
#endif