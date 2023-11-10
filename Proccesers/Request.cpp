#include "Request.hpp"

Request::Request()
{
    Content_Types = MimeTypes::GetContenTypes();
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

void Request::ParseHeaders(std::string data)
{
    std::vector<std::string> List;
    List = Lstring::Split(data, "\r\n");
    std::istringstream stream(List.at(0));
    stream >> this->Method >> this->Url;

    // for (std::vector<std::string>::iterator it = List.begin(); it != List.end(); it++)
    // {
    //     std::cout << "Cuur" << std::endl;
    //     std::vector<std::string> Attr = Lstring::Split(*it, ": ");
    //     Headers[Attr.at(0)] = Attr.at(1);
    // }
}

ResourceFile Request::Parse(std::string data)
{
    size_t index;

    if ((index = data.find("\r\n\r\n")) != std::string::npos)
    {
        ParseHeaders(data.substr(0, index));
        // this->Url = UriDecode(std::string(this->Url));
        GetResourceFilePath();
        return (this->file);
    }
    return (ResourceFile(-1));
}

std::string Request::GetHeaderAttr(std::string name)
{
    HeadersIterator it;
    it = Headers.find(name);
    if (it != Headers.end())
        return it->second;
    return ("");
}

void Request::GetResourceFilePath()
{
    std::string ResourceFilePath;
    size_t LastDotPos;

    if (Url.find("..") != std::string::npos)
        throw std::runtime_error("Invalide Url");
    ResourceFilePath = "public" + Url;
    if (Url == "/" || Url.back() == '/')
        ResourceFilePath += "index.html";
    std::cout << Lstring::Colored("ResourceFile Path ", Cyan) + Lstring::Colored(ResourceFilePath, Green) << std::endl;
    if ((LastDotPos = ResourceFilePath.find_last_of(".")) == std::string::npos)
        throw std::runtime_error("Invalide ResourceFile Name");
    this->file.ResourceFileType = this->Content_Types[ResourceFilePath.substr(LastDotPos)];
    this->file.Fd = open(ResourceFilePath.c_str(), O_RDONLY);
    if (this->file.Fd < 0)
        throw std::runtime_error("Invalide ResourceFile");
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

ResourceFile &Request::GetResourceFile()
{
    return this->file;
}