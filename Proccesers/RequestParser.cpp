#include "RequestParser.hpp"

RequestParser::RequestParser()
{
    this->Content_Types = MimeTypes::GetContenTypes();
    this->Reverse_Content_Types = MimeTypes::GetReverseContenTypes(Content_Types);
    this->Encoding = NON;
    this->Remaining = 0;
    this->BodyReady = false;
    this->dataPool.Method = OTHER;
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
    }
    /**
     *  TODO: if => location have redirection like:  return 301 /home/index.html
     */
}

bool RequestParser::Parse(std::string data)
{

    size_t index = 0;
    this->buffer.append(data);
    if ((index = buffer.find("\r\n\r\n")) != NOPOS && dataPool.Headers.empty())
    {
        ParseHeaders(buffer.substr(0, index));
        if (this->dataPool.Method == POST)
            buffer = buffer.substr(index + 4);
    }
    switch (dataPool.Method)
    {
    case GET:
        PrintfFullRequest();
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
            PrintfFullRequest();
            /**
             * TODO: if is a cgi respounce with output from cgi
             *         else responce with 201
             */
            return (dataPool.File.Fd = -1, dataPool.ResponseStatus = 201, true);
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
            DEBUGOUT(1, "BODY PARSING ENDED");
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
            // if (dataPool.body.size() > 1024)
            //     throw HTTPError(413);
        }
    }

    if (Encoding == NON)
        this->BodyReady = true;

    return (false);
}

WBSRVFILE RequestParser::SaveBodyAsFile()
{
    std::string extention;
    HeadersIterator it;
    std::string FileName;
    WBSRVFILE File;

    it = Reverse_Content_Types.find(this->GetHeaderAttr("Content-Type"));
    if (it != Reverse_Content_Types.end())
        extention = it->second;
    FileName = "public/" + Lstring::RandomStr(10) + extention;
    std::ofstream OutputFile(FileName.c_str(), std::ios::binary);
    if (!OutputFile)
    {
        DEBUGOUT(1, "Couldn't Open File : " << FileName);
        throw HTTPError(500);
    }
    OutputFile << this->dataPool.body;
    return (File);
}

WBSRVFILE RequestParser::SaveMultiPartFile(std::string &part)
{
    std::string FileName;
    std::string tmp;
    std::string Content_Type;
    WBSRVFILE File;

    FileName = Lstring::RandomStr(10);
    tmp = Lstring::ExtractFromString(part, "filename=\"", "\"");
    if (!tmp.empty())
        FileName = tmp;

    Content_Type = Lstring::ExtractFromString(part, "Content-Type: ", "\r\n");

    part = part.substr(part.find("\r\n\r\n") + 4);
    part = part.substr(0, part.size() - 2);

    std::ofstream outputFile(("public/" + FileName).c_str(), std::ios::binary);

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
    std::string Boundary;
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
        Boundary = "--" + ContentType.substr(index + Boundary.size());

        for (;;)
        {
            if ((start = this->dataPool.body.find(Boundary)) == NOPOS)
                break;
            this->dataPool.body = this->dataPool.body.substr(start + Boundary.size());
            if ((end = this->dataPool.body.find(Boundary)) == NOPOS)
                break;
            Parts.push_back(this->dataPool.body.substr(0, end));
            this->dataPool.body = this->dataPool.body.substr(end - 1);
        }
        this->dataPool.body.clear();
        for (std::vector<std::string>::iterator it = Parts.begin(); it != Parts.end(); it++)
            WBSRVFILE File = SaveMultiPartFile(*it);
        return;
    }
    SaveBodyAsFile();
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
    // Lstring::LogAsBinary(this->dataPool.body, true);
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
