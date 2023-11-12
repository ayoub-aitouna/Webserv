#include "Medium.hpp"

/**
 * TODO: Added All Parsed Values into the Medium
 *  Added Setter/Getter for all the attrebuites
 *  set them in Request parser
 *  get them in Respounce builder
 */

/*********/

Medium::Medium(ResourceFile File, int ResponseStatus)
{
    this->File = File;
    this->ResponseStatus = ResponseStatus;
}

Medium::Medium(Medium &medium)
{
    if (this != &medium)
    {
        this->File = medium.File;
        this->body = medium.body;
        this->Url = medium.Url;
        this->Query = medium.Query;
        this->Headers = medium.Headers;
        this->ResponseStatus = medium.ResponseStatus;
        this->Method = medium.Method;
    }
}

Medium::Medium(int ResponseStatus)
{
    this->File.Fd = -1;
    this->ResponseStatus = ResponseStatus;
    this->Method = OTHER;
}

Medium::Medium()
{
}

std::map<std::string, std::string> Medium::GetHeaders() const
{
    return Headers;
}

void Medium::AddHeaderAttr(std::string Name, std::string Value)
{
    Headers[Name] = Value;
}

std::string Medium::GetUrl() const
{
    return Url;
}

void Medium::SetUrl(std::string Url)
{
    Url = Url;
}

int Medium::GetMethod() const
{
    return Method;
}

void Medium::SetMethod(int Method)
{
    Method = Method;
}

int Medium::GetResponseStatus() const
{
    return ResponseStatus;
}

void Medium::SetResponseStatus(int ResponseStatus)
{
    ResponseStatus = ResponseStatus;
}

std::string Medium::GetBody() const
{
    return body;
}

void Medium::SetBody(std::string body)
{
    body = body;
}

void Medium::AppendBody(std::string buffer)
{
    this->body.append(buffer);
}

ResourceFile &Medium::GetFile()
{
    return File;
}

void Medium::SetFile(ResourceFile File)
{
    File = File;
}

std::string Medium::GetHeaderAttr(std::string name)
{
    HeadersIterator it;

    it = Headers.find(name);
    if (it != Headers.end())
        return it->second;
    return ("");
}

Medium::~Medium()
{
}
