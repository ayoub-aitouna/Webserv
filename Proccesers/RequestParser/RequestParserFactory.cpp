

#include "RequestParserFactory.hpp"

RequestParserFactory::RequestParserFactory()
{
}

RequestParser *RequestParserFactory::CreateParse(const std::string &data)
{
    std::map<std::string, Create>::iterator it;
    std::istringstream stream;
    std::string Method, Url;
    size_t index;

    if ((index = data.find("\r\n\r\n")) != std::string::npos)
    {
        stream.str(data);
        stream >> Method >> Url;
        it = parserRegistry.find(Method);
        if (it != parserRegistry.end())
            return (it->second)(Method, Url, data.substr(0, index));
    }
    return (NULL);
}

template <typename T>
void RequestParserFactory::RegisterParser(const std::string &Method, Create _parser)
{
    parserRegistry[Method] = _parser;
}
