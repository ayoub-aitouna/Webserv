#include "HttpDeserializer.hpp"

HttpDeserializer::HttpDeserializer(/* args */) : Done(false)
{
}

void HttpDeserializer::ParseHeaders(std::string headers_string)
{
    std::vector<std::string> header_lines = Lstring::Split(headers_string, "\r\n");
    requestLine.Parse(header_lines.at(0));
    std::cout << "method : " << requestLine.GetMethod() << " Url : " << requestLine.GetUrl() << " Http Version " << requestLine.GetHttpv() << std::endl;

    for (std::vector<std::string>::iterator i = header_lines.begin(); i != header_lines.end(); i++)
    {
        std::cout << ">> " << *i << std::endl;
    }
}

void HttpDeserializer::Deserialize(std::string request)
{
    size_t index;

    if ((index = request.find("\r\n\r\n")) != std::string::npos)
    {
        ParseHeaders(request.substr(0, index));
        this->Done = true;
    }
}

HttpDeserializer::~HttpDeserializer()
{
}
