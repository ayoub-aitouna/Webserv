#include "RequestParser.hpp"

RequestParser::RequestParser()
{
    Content_Types = MimeTypes::GetContenTypes();
    Encoding = NON;
    Remaining = 0;
}

std::string UriDecode(std::string Uri)
{
    std::string DecodedUrl;
    unsigned int c;
    while (true)
    {
        size_t pos = Uri.find("%");
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

void RequestParser::ParseHeaders(std::string data)
{
    Method = NON;
    std::vector<std::string> List;
    std::string strMethod;
    List = Lstring::Split(data, "\r\n");
    std::istringstream stream(List.at(0));
    stream >> strMethod >> this->Url;
    Lstring::tolower(strMethod);
    if (strMethod == "get")
        Method = Get;
    else if (strMethod == "post")
        Method = Post;
    else if (strMethod == "delete")
        Method = Delete;
    for (std::vector<std::string>::iterator it = List.begin() + 1; it != List.end(); it++)
    {
        std::vector<std::string> Attr = Lstring::Split(*it, ": ");
        Headers[Attr.at(0)] = Attr.at(1);
    }

    std::string TransferEncoding = GetHeaderAttr("Transfer-Encoding");
    std::string ContentLength = GetHeaderAttr("Content-Length");

    if (!TransferEncoding.empty() && TransferEncoding != "chunked")
        throw HTTPError(501);
    // TODO: check if URI valide
    if (Url.size() > 2048)
        throw HTTPError(414);
    if (Method == Post)
    {
        if (TransferEncoding == "chunked")
            Encoding = Chunked;
        else if (!ContentLength.empty())
            Encoding = Lenght;
        /**
         *  TODO: if => RequestParser body larger than client max body size in config file
         *        change <1024> with max-body from config file */
        if (Encoding == Lenght && (Remaining = atoi(ContentLength.c_str()) > 1024))
            throw HTTPError(413);
    }
    // TODO: if => location have redirection like:  return 301 /home/index.html
}

Medium *RequestParser::Parse(std::string data)
{
    size_t index;

    if ((index = data.find("\r\n\r\n")) != std::string::npos && Headers.empty())
        ParseHeaders(data.substr(0, index));
    if (Method == Get)
    {
        GetResourceFilePath();
        return (new Medium(this->File, 200));
    }
    if (Method == Post)
        return (ParseBody(data));
    if (Method == Delete)
        return (ParseBody(data));
    return (NULL);
}

Medium *RequestParser::ParseBody(std::string data)
{
    // body size checked in headers parsing in case of encoding = Lenght
    if (Remaining != 0 && Encoding == Lenght)
    {
        this->body.append(data);
        Remaining -= data.size();
        if (Remaining == 0)
            return (new Medium(this->File, 200));
    }

    if (Encoding == Chunked)
    {
        size_t index;
        tmp.append(data);

        while ((index = tmp.find("\r\n")) != std::string::npos)
        {
            Remaining = strtol(tmp.c_str(), 0, 16);
            if (Remaining == 0)
                return (new Medium(this->File, 200));
            body.append(tmp.substr(index + 2, Remaining));
            tmp = tmp.substr(index + Remaining + 4);
            /**
             *  TODO: if => RequestParser body larger than client max body size in config file
             *        change <1024> with max-body from config file */
            if (body.size() > 1024)
                throw HTTPError(413);
        }
    }
    return (NULL);
}

std::string RequestParser::GetHeaderAttr(std::string name)
{
    HeadersIterator it;

    it = Headers.find(name);
    if (it != Headers.end())
        return it->second;
    return ("");
}

void RequestParser::GetResourceFilePath()
{
    std::string ResourceFilePath;
    size_t LastDotPos;

    if (Url.find("..") != std::string::npos)
        throw HTTPError(403);
    ResourceFilePath = "public" + Url;
    if (Url == "/" || *(Url.end() - 1) == '/')
        ResourceFilePath += "index.html";
    std::cout << Lstring::Colored("ResourceFile Path ", Cyan) + Lstring::Colored(ResourceFilePath, Green) << std::endl;
    if ((LastDotPos = ResourceFilePath.find_last_of(".")) == std::string::npos)
        throw HTTPError(404);
    this->File.ResourceFileType = this->Content_Types[ResourceFilePath.substr(LastDotPos)];
    this->File.Fd = open(ResourceFilePath.c_str(), O_RDONLY);
    if (this->File.Fd < 0)
        throw HTTPError(404);
}
