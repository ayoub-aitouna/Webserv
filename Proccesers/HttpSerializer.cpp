#include "HttpSerializer.hpp"

HttpSerializer::HttpSerializer(/* args */) : DoneDeserializing(false)
{
}

void HttpSerializer::ParseHeaders(std::string headers_string)
{
    std::vector<std::string> header_lines;
    std::vector<std::string> current_header;

    header_lines = Lstring::Split(headers_string, "\r\n");
    requestLine.Parse(header_lines.at(0));

    for (std::vector<std::string>::iterator i = header_lines.begin() + 1; i != header_lines.end(); i++)
    {
        current_header = Lstring::Split(*i, ": ");
        this->requestLine.GetRequestHeaders()[current_header.at(0)] = current_header.at(1);
        std::cout << Lstring::Colored(current_header.at(0), Blue) << " :: " << Lstring::Colored(current_header.at(1), Green) << std::endl;
    }

    std::cout << "method : " << requestLine.GetMethod() << " Url : " << requestLine.GetUrl() << " Http Version " << requestLine.GetHttpv() << std::endl;
}

void HttpSerializer::Deserialize(std::string request)
{
    size_t index;

    if ((index = request.find("\r\n\r\n")) != std::string::npos)
    {
        ParseHeaders(request.substr(0, index));
        this->DoneDeserializing = true;
    }
}

std::string HttpSerializer::Serialize()
{
    return ("");
}

HttpSerializer::~HttpSerializer()
{
}
