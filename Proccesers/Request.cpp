#include "Request.hpp"
#include <iostream>

Request::Request()
{
}

std::string UriDecode(std::string Uri)
{
    std::string DecodedUrl;
    unsigned int c;
    while (true)
    {
        int pos = Uri.find("%");
        if (pos == std::string::npos)
            break;
        DecodedUrl.append(Uri.substr(0, pos));
        std::istringstream hexCode(Uri.substr(pos + 1, 2));
        hexCode >> std::hex >> c;
        DecodedUrl += static_cast<char>(c);
        Uri = Uri.substr(pos + 3);
    }
    return (DecodedUrl);
}

void Request::Parse(std::string line)
{
    std::istringstream stream;
    stream.str(line);
    stream >> this->Method >> this->Url;
    // this->Url = UriDecode(std::string(this->Url));
}

const std::string &Request::GetMethod() const
{
    return (this->Method);
}

const std::string &Request::GetUrl() const
{
    return (this->Url);
}

std::string &Request::GetBody()
{
    return (this->body);
}