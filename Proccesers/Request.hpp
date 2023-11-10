#ifndef Request_HPP
#define Request_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "../Lib/Lstring.hpp"

class Request
{
public:
    Request();
    void Parse(std::string line);

private:
    std::string Method;
    std::string Url;
    std::string body;

public:
    const std::string &GetMethod() const;
    const std::string &GetUrl() const;
    std::string &GetBody();
};
#endif