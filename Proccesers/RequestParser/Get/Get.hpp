#ifndef GETREQUEST_HPP
#define GETREQUEST_HPP

#include <string>
#include "../RequestParser.hpp"

class Get : public RequestParser
{
public:
    Get();
    Get(std::string Method, std::string Url, std::string Headers);
    RequestParser *Create(std::string Method, std::string Url, std::string Headers);
    void Parse(std::string data);
    ~Get();
};
#endif