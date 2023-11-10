#include "HttpSerializer.hpp"

HttpSerializer::HttpSerializer()
{
    response = NULL;
}

void HttpSerializer::ParseHeaders(std::string headers_string)
{
    std::vector<std::string> header_lines;
    std::vector<std::string> current_header;

    header_lines = Lstring::Split(headers_string, "\r\n");
    request.Parse(header_lines.at(0));
}

void HttpSerializer::Deserialize(std::string data)
{
    size_t index;

    if ((index = data.find("\r\n\r\n")) != std::string::npos)
    {
        ParseHeaders(data.substr(0, index));
        response = new Response(this->request);
    }
}

std::string HttpSerializer::Serialize()
{
    return ("");
}

Request &HttpSerializer::GetRequest()
{
    return (this->request);
}

Response *HttpSerializer::GetResponse()
{
    return (this->response);
}

HttpSerializer::~HttpSerializer()
{
}
