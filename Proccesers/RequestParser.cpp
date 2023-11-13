#include "RequestParser.hpp"

RequestParser::RequestParser()
{
    Content_Types = MimeTypes::GetContenTypes();
    Reverse_Content_Types = MimeTypes::GetReverseContenTypes(Content_Types);
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
        if (pos == NOPOS)
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
    stream >> strMethod >> this->dataPool.Url;
    Lstring::tolower(strMethod);
    this->dataPool.Method = strMethod == "get" ? GET : (strMethod == "post") ? POST
                                                                             : DELETE;

    DEBUGOUT(0, "Method is " << strMethod);

    for (std::vector<std::string>::iterator it = List.begin() + 1; it != List.end(); it++)
    {
        std::vector<std::string> Attr = Lstring::Split(*it, ": ");
        this->dataPool.Headers[Attr.at(0)] = Attr.at(1);
    }

    std::string TransferEncoding = GetHeaderAttr("Transfer-Encoding");
    std::string ContentLength = GetHeaderAttr("Content-Length");

    if (ContentLength.empty() && TransferEncoding.empty() && dataPool.Method == POST)
        throw HTTPError(501);

    if (!TransferEncoding.empty() && TransferEncoding != "chunked")
        throw HTTPError(501);

    /**
     *  TODO: check if URI valide
     */

    if (Url.size() > 2048)
        throw HTTPError(414);

    if (dataPool.Method == POST)
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
            if (Remaining > 5 * 1024 * KB)
                throw HTTPError(413);
        }
        DEBUGOUT(0, "Remaining >> " << atoi(ContentLength.c_str()) << " ContentLength  >> "
                                    << ContentLength << "  Encoding " << (Encoding == 0 ? "Lenght" : "OTHER"));
    }
    /**
     *  TODO: if => location have redirection like:  return 301 /home/index.html
     */
}

bool RequestParser::Parse(std::string data)
{
    size_t index = 0;
    this->buffer.append(data);
    DEBUGOUT(1, "START PARSING ..  " << data.size());
    if ((index = buffer.find("\r\n\r\n")) != NOPOS && dataPool.Headers.empty())
    {
        DEBUGOUT(1, "START PARSING HEADERS..");
        ParseHeaders(buffer.substr(0, index));
        buffer = buffer.substr(index + 4);
    }
    switch (dataPool.Method)
    {
    case GET:
        GetResourceFilePath();
        return (dataPool.ResponseStatus = 200, true);
        break;

    case POST:
        if (!this->BodyReady)
            this->BodyReady = FillBody(buffer);
        if (this->BodyReady)
        {
            ParseBody();
            /** TODO:: Write Body into file
             * */
            // PrintfFullRequest();

            /**
             * TODO: if is a cgi respounce wthi output f cgi
             *         else responce with 201
             */
            GetResourceFilePath();
            return (dataPool.ResponseStatus = 200, true);
        }
        break;

    case DELETE:
        /**
         * TODO:
         *  */
        break;
    default:;
        return (dataPool.ResponseStatus = 501, true);
        break;
    }
    return (false);
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
            this->dataPool.body.append(data.substr(0, PartSize));
            Remaining -= PartSize;
            data = data.substr(PartSize);
        }
        if (Remaining == 0)
        {
            DEBUGOUT(0, "BODY PARSING ENDED");
            return (true);
        }
    }

    if (Encoding == Chunked)
    {
        size_t index;
        size_t Part;

        while ((index = data.find("\r\n")) != NOPOS)
        {
            Remaining = strtol(data.c_str(), 0, 16);
            if (Remaining == 0)
                return (true);
            Part = ((size_t)Remaining) > data.size() ? data.size() : Remaining;
            dataPool.body.append(data.substr(index + 2, Part));
            DEBUGOUT(0, COLORED(data.substr(index + 2, Part), Red));
            if ((index + Remaining + 4) > data.size())
                data.clear();
            else
                data = data.substr((index + Remaining + 4));
            /**
             *  TODO: if => RequestParser body larger than client max body size in config file
             *        change <1024> with max-body from config file */
            if (dataPool.body.size() > 1024)
                throw HTTPError(413);
        }
    }

    if (Encoding == NON)
        this->BodyReady = true;

    return (false);
}

WBSRVFILE RequestParser::SaveAsFile(std::string &part)
{
    std::string FileName;
    std::string Content_Type;
    WBSRVFILE File;
    FileName = "FILE.txt";
    FileName = Lstring::ExtractFromString(part, "filename=\"", "\"");
    if (FileName.empty())
        FileName = "FILE.txt";
    Content_Type = Lstring::ExtractFromString(part, "Content-Type: ", "\r\n");
    part = part.substr(part.find("\r\n\r\n") + 4);
    part = part.substr(0, part.find("\r\n"));

    std::ofstream outputFile(("public/" + FileName).c_str(), std::ios::out | std::ios::binary);
    if (!outputFile)
    {
        DEBUGOUT(1, "Error While Opening File " << FileName);
        throw HTTPError(500);
    }
    outputFile << part;
    return (File);
}

void RequestParser::ParseBody()
{
    std::string Boundary, Devider, EOB;
    size_t index, start, end;

    std::string ContentType;
    std::vector<std::string> Parts;

    DEBUGOUT(1, std::endl
                    << COLORED("PARSING BODY", Magenta));
    Boundary = "boundary=";
    ContentType = GetHeaderAttr("Content-Type");
    if (ContentType.empty())
        return;
    if ((index = ContentType.find(Boundary)) != NOPOS)
    {
        /**
         * TODO: parse multipart/form-data body
         */
        Boundary = ContentType.substr(index + Boundary.size());
        Devider = "--" + Boundary;
        EOB = Boundary + "--\r\n";

        for (;;)
        {
            if ((start = this->dataPool.body.find(Devider)) == NOPOS)
                break;
            this->dataPool.body = this->dataPool.body.substr(start + Devider.size());
            if ((end = this->dataPool.body.find(Devider)) == NOPOS)
                break;

            Parts.push_back(this->dataPool.body.substr(0, end));
            this->dataPool.body = this->dataPool.body.substr(end - 1);
        }
        this->dataPool.body.clear();
        for (std::vector<std::string>::iterator it = Parts.begin(); it != Parts.end(); it++)
        {
            WBSRVFILE File = SaveAsFile(*it);
        }
    }
}

std::string RequestParser::GetHeaderAttr(std::string name)
{
    HeadersIterator it;

    it = dataPool.Headers.find(name);
    if (it != dataPool.Headers.end())
        return it->second;
    return ("");
}

void RequestParser::PrintfFullRequest()
{
    for (HeadersIterator i = dataPool.Headers.begin(); i != dataPool.Headers.end(); i++)
        DEBUGOUT(1, COLORED(i->first, Magenta) << " : " << COLORED(i->second, Green));
    DEBUGOUT(1, std::endl);
    DEBUGOUT(1, COLORED(SSTR(dataPool.body.size()), Green));
    DEBUGOUT(1, std::endl);
    DEBUGOUT(1, COLORED(dataPool.body, Magenta));
}

void RequestParser::GetResourceFilePath()
{

    /**
     * TODO: get_auto_index() checks if config-ON
     * list all directory content in html
     */

    std::string ResourceFilePath;
    size_t LastDotPos;

    if (dataPool.Url.find("..") != NOPOS)
        throw HTTPError(403);
    ResourceFilePath = "public" + dataPool.Url;
    if (dataPool.Url == "/" || *(dataPool.Url.end() - 1) == '/')
        ResourceFilePath += "index.html";
    DEBUGOUT(0, COLORED("ResourceFile Path ", Cyan) << COLORED(ResourceFilePath, Green));
    if ((LastDotPos = ResourceFilePath.find_last_of(".")) == NOPOS)
        throw HTTPError(404);
    this->dataPool.File.ResourceFileType = this->Content_Types[ResourceFilePath.substr(LastDotPos)];
    this->dataPool.File.Fd = open(ResourceFilePath.c_str(), O_RDONLY);
    if (this->dataPool.File.Fd < 0)
        throw HTTPError(404);
}
