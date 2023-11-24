#include "Include/RequestParser.hpp"

RequestParser::RequestParser()
{
    dataPool.Method = OTHER;
    dataPool.File.Fd = NOBODY;
    dataPool.ResponseStatus = 0;
    this->RequestHandler = NULL;
    dataPool.ServerConf = NULL;
}

void RequestParser::ParseUrl(std::string &Url)
{
    std::string DecodedUrl;
    unsigned int c;
    size_t index;

    if (dataPool.Url.size() > 2048)
        throw HTTPError(414);

    if (dataPool.Url.find("..") != std::string::npos)
        throw HTTPError(403);
    while ((index = Url.find("%")) != std::string::npos)
    {
        DecodedUrl.append(Url.substr(0, index));
        std::istringstream hexCode(Url.substr(index + 1, 2));
        hexCode >> std::hex >> c;
        DecodedUrl += static_cast<char>(c);
        Url = Url.substr(index + 3);
    }
    DecodedUrl.append(Url);
    Url = DecodedUrl;

    if ((index = Url.find("?")) != std::string::npos)
    {
        dataPool.Query = Url.substr(index + 1);
        DEBUGOUT(1, COLORED(dataPool.Query, Yellow));
        dataPool.Url = Url.substr(0, index);
    }
}

void RequestParser::RequestHandlersFactory(std::string &Method)
{
    if (Method == "get")
    {
        dataPool.Method = GET;
        RequestHandler = new GetRequest(dataPool);
        return;
    }
    if ((Method == "post"))
    {
        dataPool.Method = POST;
        RequestHandler = new PostRequest(dataPool);
        return;
    }
    if ((Method == "delete"))
    {
        DEBUGOUT(1, COLORED("DELETE -  Is DIR", Red));
        dataPool.Method = DELETE;
        RequestHandler = new DeleteRequest(dataPool);
        return;
    }
    RequestHandler = NULL;
    dataPool.Method = OTHER;
}

void RequestParser::ParseHeaders(std::string data)
{
    std::vector<std::string> List;
    std::string MethodName;
    std::string TransferEncoding;
    std::string ContentLength;
    std::istringstream stream;

    List = Lstring::Split(data, CRLF);
    stream.str((List.at(0)));
    stream >> MethodName >> dataPool.Url;
    MethodName = Lstring::tolower(MethodName);
    ParseUrl(dataPool.Url);
    DEBUGOUT(1, "Requested :: " << dataPool.Url << " Method " << MethodName);

    for (std::vector<std::string>::iterator it = List.begin() + 1; it != List.end(); it++)
    {
        std::vector<std::string> Attr = Lstring::Split(*it, ": ");
        dataPool.Headers[Attr.at(0)] = Attr.at(1);
    }

    TransferEncoding = GetHeaderAttr(dataPool.Headers, "Transfer-Encoding");
    ContentLength = GetHeaderAttr(dataPool.Headers, "Content-Length");

    if (ContentLength.empty() && TransferEncoding.empty() && dataPool.Method == POST)
        throw HTTPError(501);

    if (!TransferEncoding.empty() && TransferEncoding != "chunked")
        throw HTTPError(501);

    if (!dataPool.ServerConf &&
        !(dataPool.ServerConf = ConfigHandler::GetHttp().GetServersByHost(GetHeaderAttr(dataPool.Headers, "Host"))))
        throw HTTPError(500);

    dataPool.ServerConf->SetRequestPath(this->dataPool.Url);

    if (!dataPool.ServerConf->GetAllowed().empty() && !Containes(dataPool.ServerConf->GetAllowed(), MethodName))
        throw HTTPError(405);

    RequestHandlersFactory(MethodName);
    if (TransferEncoding == "chunked")
        this->RequestHandler->SetBodyController(Chunked, 0);
    else if (!ContentLength.empty())
        this->RequestHandler->SetBodyController(Lenght, atoll(ContentLength.c_str()));
}

bool RequestParser::Parse(std::string data)
{
    size_t index;
    this->buffer.append(data);
    if ((index = buffer.find(DBLCRLF)) != std::string::npos && dataPool.Headers.empty())
    {
        ParseHeaders(buffer.substr(0, index));
        buffer = buffer.substr(index + 4);
    }
    if (!this->RequestHandler)
        return (false);
    return (this->RequestHandler->HandleRequest(this->buffer));
}

Request *RequestParser::GetRequestHandler()
{
    return this->RequestHandler;
}

std::string GetHeaderAttr(std::map<std::string, std::string> &Headers, std::string name)
{
    HeadersIterator it;

    it = Headers.find(name);
    if (it != Headers.end())
        return it->second;
    return ("");
}

RequestParser::~RequestParser()
{
    delete this->RequestHandler;
}