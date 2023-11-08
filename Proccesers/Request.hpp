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
    Request(std::string line);
    Request();
    void Parse(std::string line);

private:
    std::string Method;
    std::string Url;
    std::string Httpv;

public:
    const std::string &GetMethod() const;
    const std::string &GetUrl() const;
    const std::string &GetHttpv() const;
};
#endif