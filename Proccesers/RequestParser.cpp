#include "RequestParser.hpp"

RequestParser::RequestParser()
{
    Content_Types = MimeTypes::GetContenTypes();
    Encoding = NON;
    Remaining = 0;
    BodyReady = false;
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

    std::vector<std::string> List;
    std::string strMethod;
    List = Lstring::Split(data, "\r\n");
    std::istringstream stream(List.at(0));

    /**
     * TODO: improve URL parsing and extract the query
     * check if folder/file
     */
    std::string Url;
    stream >> strMethod >> Url;
    this->medium.SetUrl(Url);
    Lstring::tolower(strMethod);
    this->medium.SetMethod(strMethod == "get"
                               ? Get
                           : (strMethod == "post") ? Post
                                                   : Delete);

    DEBUGOUT(0, "Method is " << strMethod);

    for (std::vector<std::string>::iterator it = List.begin() + 1; it != List.end(); it++)
    {
        std::vector<std::string> Attr = Lstring::Split(*it, ": ");
        medium.AddHeaderAttr(Attr.at(0), Attr.at(1));
    }

    std::string TransferEncoding = medium.GetHeaderAttr("Transfer-Encoding");
    std::string ContentLength = medium.GetHeaderAttr("Content-Length");

    if (ContentLength.empty() && TransferEncoding.empty() && medium.GetMethod() == Post)
        throw HTTPError(501);

    if (!TransferEncoding.empty() && TransferEncoding != "chunked")
        throw HTTPError(501);

    /**
     *  TODO: check if URI valide
     */
    if (Url.size() > 2048)
        throw HTTPError(414);

    if (medium.GetMethod() == Post)
    {
        if (TransferEncoding == "chunked")
            Encoding = Chunked;
        else if (!ContentLength.empty())
            Encoding = Lenght;

        /**
         *  TODO: if => RequestParser body larger than client max body size in config file
         *        change <2024> with max-body from config file */
        if (Encoding == Lenght)
        {
            Remaining = atoi(ContentLength.c_str());
            if (Remaining > 2024)
                throw HTTPError(413);
        }
        DEBUGOUT(0, "Remaining >> " << atoi(ContentLength.c_str()) << " ContentLength  >> "
                                    << ContentLength << "  Encoding " << (Encoding == 0 ? "Lenght" : "OTHER"));
    }
    /**
     *  TODO: if => location have redirection like:  return 301 /home/index.html
     */
}

Medium *RequestParser::Parse(std::string data)
{
    size_t index = 0;
    this->buffer.append(data);
    DEBUGOUT(0, "START PARSING ..  " << data.size());
    if ((index = buffer.find("\r\n\r\n")) != std::string::npos && medium.GetHeaders().empty())
    {
        DEBUGOUT(0, "START PARSING HEADERS..");
        ParseHeaders(buffer.substr(0, index));
        buffer = buffer.substr(index + 4);
    }
    switch (medium.GetMethod())
    {
    case Get:
        GetResourceFilePath();
        medium.SetResponseStatus(200);
        return (new Medium(medium));
        break;

    case Post:
        if (!this->BodyReady)
            this->BodyReady = FillBody(buffer);
        else
        {
            ParseBody();
            /** TODO:: Write Body into file
             * */

            PrintfFullRequest();

            /**
             * TODO: if is a cgi respounce wthi output f cgi
             *         else responce with 201
             */
            GetResourceFilePath();
            medium.SetResponseStatus(200);
            return (new Medium(medium));
        }
        break;

    case Delete:
        /**
         * TODO:
         *  */
        break;
    default:
        medium.SetResponseStatus(501);
        return (new Medium(medium));
        break;
    }
    return (NULL);
}

void RequestParser::ParseBody()
{
    DEBUGOUT(1, std::endl
                    << COLORED("PARSING BODY", Magenta));

    std::string ContentType = medium.GetHeaderAttr("Content-Type");

    size_t index;
    if (ContentType.empty())
        return;
    if ((index = ContentType.find("boundary=")) != std::string::npos)
    {
        std::string boundary = ContentType.substr(index + 9);
        DEBUGOUT(1, "boundary=" << COLORED(boundary, Magenta));
        std::vector<std::string> Parts = Lstring::Split(this->medium.GetBody(), boundary);
        for (std::vector<std::string>::iterator it = Parts.begin(); it != Parts.end(); it++)
            DEBUGOUT(1, COLORED(*it, Yellow));

        /**
         * TODO: parse multipart/form-data body
         */
    }
}

int RequestParser::FillBody(std::string &data)
{

    if (Encoding == Lenght)
    {
        DEBUGOUT(0, std::endl
                        << COLORED("FILLING BODY FROM data ... REMAINING :: ", Magenta) << Remaining);
        if (Remaining != 0)
        {
            size_t PartSize = data.size() > (size_t)Remaining ? Remaining : data.size();
            DEBUGOUT(0, COLORED(data.substr(0, PartSize), Red));
            this->medium.GetBody().append(data.substr(0, PartSize));
            Remaining -= PartSize;
            DEBUGOUT(0, std::endl
                            << COLORED("New ... REMAINING :: ", Magenta)
                            << Remaining << COLORED("PartSize :: ", Magenta)
                            << PartSize);
            data = data.substr(PartSize);
        }
        if (Remaining == 0)
            return (true);
    }

    if (Encoding == Chunked)
    {
        size_t index;
        size_t Part;

        while ((index = data.find("\r\n")) != std::string::npos)
        {
            Remaining = strtol(data.c_str(), 0, 16);
            if (Remaining == 0)
                return (true);
            Part = ((size_t)Remaining) > data.size() ? data.size() : Remaining;
            medium.GetBody().append(data.substr(index + 2, Part));
            DEBUGOUT(0, COLORED(data.substr(index + 2, Part), Red));
            if ((index + Remaining + 4) > data.size())
                data.clear();
            else
                data = data.substr((index + Remaining + 4));
            /**
             *  TODO: if => RequestParser body larger than client max body size in config file
             *        change <1024> with max-body from config file */
            if (medium.GetBody().size() > 1024)
                throw HTTPError(413);
        }
    }

    if (Encoding == NON)
        this->BodyReady = true;

    return (false);
}

void RequestParser::PrintfFullRequest()
{
    for (HeadersIterator i = medium.GetHeaders().begin(); i != medium.GetHeaders().end(); i++)
        DEBUGOUT(1, COLORED(i->first, Magenta) << " : " << COLORED(i->second, Green));
    DEBUGOUT(1, std::endl);
    DEBUGOUT(1, COLORED(SSTR(medium.GetBody().size()), Green));
    DEBUGOUT(1, std::endl);
    DEBUGOUT(1, COLORED(medium.GetBody(), Magenta));
}

void RequestParser::GetResourceFilePath()
{

    /**
     * TODO: get_auto_index() checks if config-ON
     * list all directory content in html
     */

    std::string ResourceFilePath;
    size_t LastDotPos;

    if (medium.GetUrl().find("..") != std::string::npos)
        throw HTTPError(403);
    ResourceFilePath = "public" + medium.GetUrl();
    if (medium.GetUrl() == "/" || *(medium.GetUrl().end() - 1) == '/')
        ResourceFilePath += "index.html";
    DEBUGOUT(0, COLORED("ResourceFile Path ", Cyan) << COLORED(ResourceFilePath, Green));
    if ((LastDotPos = ResourceFilePath.find_last_of(".")) == std::string::npos)
        throw HTTPError(404);
    this->medium.GetFile().ResourceFileType = this->Content_Types[ResourceFilePath.substr(LastDotPos)];
    this->medium.GetFile().Fd = open(ResourceFilePath.c_str(), O_RDONLY);
    if (this->medium.GetFile().Fd < 0)
        throw HTTPError(404);
}
