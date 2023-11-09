#include "Request.hpp"
#include <iostream>

Request::Request()
{
}

Request::Request(std::string line)
{
    Parse(line);
}

void Request::Parse(std::string line)
{
    std::istringstream stream;
    stream.str(line);
    stream >> this->Method >> this->Url >> this->Httpv;
}

const std::string &Request::GetMethod() const
{
    return (this->Method);
}

const std::string &Request::GetUrl() const
{
    return (this->Url);
}

const std::string &Request::GetHttpv() const
{
    return (this->Httpv);
}

std::map<std::string, std::string> &Request::GetRequestHeaders()
{
    return (this->RequestHeaders);
}

std::string &Request::GetBody()
{
    return (this->body);
}