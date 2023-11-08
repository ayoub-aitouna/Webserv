#include "Request.hpp"
Request::Request() : Method(""), Url(""), Httpv("")
{
}

Request::Request(std::string line)
{
    Parse(line);
}

void Request::Parse(std::string line)
{
    std::vector<std::string> request_list = Lstring::Split(line, " ");
    if (request_list.size() != 3)
        throw std::runtime_error("Request Line Invalide");
    this->Method = request_list.at(0);
    this->Url = request_list.at(1);
    this->Httpv = request_list.at(2);
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