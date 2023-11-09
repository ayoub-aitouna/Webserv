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

    for (std::vector<std::string>::iterator i = header_lines.begin() + 1; i != header_lines.end(); i++)
    {
        current_header = Lstring::Split(*i, ": ");
        this->request.GetRequestHeaders()[current_header.at(0)] = current_header.at(1);
        std::cout << Lstring::Colored(current_header.at(0), Blue) << " :: " << Lstring::Colored(current_header.at(1), Green) << std::endl;
    }

    std::cout << "method : " << request.GetMethod() << " Url : " << request.GetUrl() << " Http Version " << request.GetHttpv() << std::endl;
}

void HttpSerializer::Deserialize(std::string request)
{
    size_t index;

    if ((index = request.find("\r\n\r\n")) != std::string::npos)
    {
        ParseHeaders(request.substr(0, index));
        response = new Response(request);
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
