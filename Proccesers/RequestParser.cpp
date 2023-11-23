#include "Include/RequestParser.hpp"

RequestParser::RequestParser()
{
    this->dataPool.Method = OTHER;
    this->dataPool.File.Fd = NOBODY;
    this->RequestHandler = NULL;
    this->dataPool.ServerConf  = NULL;
}

void RequestParser::ParseUrl(std::string &Url)
{
    std::string DecodedUrl;
    unsigned int c;
    size_t index;

    if (this->dataPool.Url.size() > 2048)
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
        this->dataPool.Query = Url.substr(index + 1);
        DEBUGOUT(1, COLORED(this->dataPool.Query, Yellow));
        this->dataPool.Url = Url.substr(0, index);
    }

    DEBUGOUT(1, "Requested :: " << this->dataPool.Url);
}

void RequestParser::RequestHandlersFactory(std::string &Method)
{
    if (Method == "get")
    {
        this->dataPool.Method = GET;
        RequestHandler = new GetRequest(this->dataPool);
        return;
    }
    if ((Method == "post"))
    {
        this->dataPool.Method = POST;
        RequestHandler = new PostRequest(this->dataPool);
        return;
    }
    if ((Method == "delete"))
    {
        this->dataPool.Method = DELETE;
        RequestHandler = new DeleteRequest(this->dataPool);
        return;
    }
    RequestHandler = NULL;
    this->dataPool.Method = OTHER;
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
    stream >> MethodName >> this->dataPool.Url;

    ParseUrl(this->dataPool.Url);

    for (std::vector<std::string>::iterator it = List.begin() + 1; it != List.end(); it++)
    {
        std::vector<std::string> Attr = Lstring::Split(*it, ": ");
        // if headr Attr == "Cockie" added in Cockies_vector
        // to be passed later to cgi
        this->dataPool.Headers[Attr.at(0)] = Attr.at(1);
    }

    TransferEncoding = GetHeaderAttr(this->dataPool, "Transfer-Encoding");
    ContentLength = GetHeaderAttr(this->dataPool, "Content-Length");

    if (ContentLength.empty() && TransferEncoding.empty() && dataPool.Method == POST)
        throw HTTPError(501);

    if (!TransferEncoding.empty() && TransferEncoding != "chunked")
        throw HTTPError(501);

    RequestHandlersFactory(Lstring::tolower(MethodName));
    if (TransferEncoding == "chunked")
        this->RequestHandler->SetBodyController(Chunked, 0);
    else if (!ContentLength.empty())
        this->RequestHandler->SetBodyController(Lenght, atoll(ContentLength.c_str()));
    if (!this->dataPool.ServerConf)
    {

        if (!(this->dataPool.ServerConf = ConfigHandler::GetHttp()
                                              .GetServersByHost(GetHeaderAttr(this->dataPool, "Host"))))
            throw HTTPError(500);
    }
    if (this->dataPool.ServerConf == NULL)
    {
        DEBUGOUT(1, COLORED("SERVER CONFIG CLASS IS NULL", Red));
    }
    this->dataPool.ServerConf->DisplayValues(true);
    /**
     *  TODO: if => location have redirection like:  return 301 /home/index.html
     */
}

bool RequestParser::Parse(std::string data)
{
    size_t index;
    DEBUGOUT(0, "HERE");
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

std::string GetHeaderAttr(DataPool &dataPool, std::string name)
{
    HeadersIterator it;

    it = dataPool.Headers.find(name);
    if (it != dataPool.Headers.end())
        return it->second;
    return ("");
}

RequestParser::~RequestParser()
{
    delete this->RequestHandler;
}