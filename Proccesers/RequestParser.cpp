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
    Method = OTHER;
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

    DEBUGOUT(0, "Method is " << strMethod);

    for (std::vector<std::string>::iterator it = List.begin() + 1; it != List.end(); it++)
    {
        std::vector<std::string> Attr = Lstring::Split(*it, ": ");
        Headers[Attr.at(0)] = Attr.at(1);
    }

    std::string TransferEncoding = GetHeaderAttr("Transfer-Encoding");
    std::string ContentLength = GetHeaderAttr("Content-Length");

    if (ContentLength.empty() && TransferEncoding.empty() && Method == Post)
        throw HTTPError(501);

    if (!TransferEncoding.empty() && TransferEncoding != "chunked")
        throw HTTPError(501);

    /**
     *  TODO: check if URI valide
     */
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
        if (Encoding == Lenght)
        {
            Remaining = atoi(ContentLength.c_str());
            // if (Remaining > 1024)
            //     throw HTTPError(413);
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
    if ((index = buffer.find("\r\n\r\n")) != std::string::npos && Headers.empty())
    {
        DEBUGOUT(0, "START PARSING HEADERS..");
        ParseHeaders(buffer.substr(0, index));
        buffer = buffer.substr(index + 4);
    }

    if (Method == Get)
    {
        GetResourceFilePath();
        return (new Medium(this->File, 200));
    }
    else if (Method == Post && !BodyReady)
    {
        this->BodyReady = FillBody(buffer);
        // std::cout <<
        if (this->BodyReady)
        {
            ParseBody();
            PrintfFullRequest();
            GetResourceFilePath();
            return (new Medium(this->File, 200));
        }
    }
    else if (Method == Delete)
    {
        // return (FillBody(buffer));
    }

    return (NULL);
}

void RequestParser::ParseBody()
{
    DEBUGOUT(1, std::endl
                    << COLORED("PARSING BODY", Magenta));

    std::string ContentType = GetHeaderAttr("Content-Type");

    size_t index;
    if (ContentType.empty())
        return;
    if ((index = ContentType.find("boundary=")) != std::string::npos)
    {
        std::string boundary = ContentType.substr(index + 9);
        DEBUGOUT(1, "boundary=" << COLORED(boundary, Magenta));
        std::vector<std::string> Parts = Lstring::Split(this->body, boundary);
        for (std::vector<std::string>::iterator it = Parts.begin(); it != Parts.end(); it++)
            DEBUGOUT(1, COLORED(*it, Yellow));

        /**
         * TODO: parse multipart/form-data body
         */
    }
    /**
     * TODO: handle other types of the body
     */
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
            this->body.append(data.substr(0, PartSize));
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
            body.append(data.substr(index + 2, Part));
            DEBUGOUT(0, COLORED(data.substr(index + 2, Part), Red));
            if ((index + Remaining + 4) > data.size())
                data.clear();
            else
                data = data.substr((index + Remaining + 4));
            /**
             *  TODO: if => RequestParser body larger than client max body size in config file
             *        change <1024> with max-body from config file */
            if (body.size() > 1024)
                throw HTTPError(413);
        }
    }

    if (Encoding == NON)
        this->BodyReady = true;

    return (false);
}

void RequestParser::PrintfFullRequest()
{
    for (HeadersIterator i = Headers.begin(); i != Headers.end(); i++)
        DEBUGOUT(1, COLORED(i->first, Magenta) << " : " << COLORED(i->second, Green));
    DEBUGOUT(1, std::endl);
    DEBUGOUT(1, COLORED(SSTR(body.size()), Green));
    DEBUGOUT(1, std::endl);
    DEBUGOUT(1, COLORED(this->body, Magenta));
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
    DEBUGOUT(0, COLORED("ResourceFile Path ", Cyan) << COLORED(ResourceFilePath, Green));
    if ((LastDotPos = ResourceFilePath.find_last_of(".")) == std::string::npos)
        throw HTTPError(404);
    this->File.ResourceFileType = this->Content_Types[ResourceFilePath.substr(LastDotPos)];
    this->File.Fd = open(ResourceFilePath.c_str(), O_RDONLY);
    if (this->File.Fd < 0)
        throw HTTPError(404);
}
