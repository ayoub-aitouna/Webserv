#include "Get.hpp"
#include "../RequestParser.hpp"

Get::Get(std::string Method, std::string Url, std::string Headers)
{
    this->method = Method;
    this->url = Url;
}

void Get::Parse(std::string data)
{
}

RequestParser *Get::Create(std::string Method, std::string Url, std::string Headers)
{
    return (new Get(Method, Url, Headers));
}

Get::~Get()
{
}
