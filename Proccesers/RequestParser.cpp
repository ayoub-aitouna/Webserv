#include "RequestParser.hpp"

RequestParser::RequestParser()
{
    this->dataPool.Content_Types = MimeTypes::GetContenTypes();
    this->dataPool.Reverse_Content_Types = MimeTypes::GetReverseContenTypes(dataPool.Content_Types);
    this->BodyReady = false;
    this->dataPool.Method = OTHER;
    this->dataPool.File.Fd = NOBODY;
}

/**
 * TODO: improve URL parsing and extract the query
 * check if folder/file
 */
void RequestParser::ParseUrl(std::string &Url)
{
    std::string DecodedUrl;
    unsigned int c;
    size_t index;

    if (dataPool.Url.find("..") != std::string::npos)
        throw HTTPError(403);
    if ((index = Url.find("?")) != std::string::npos)
    {
        this->dataPool.Query = Url.substr(index + 1);
        this->dataPool.Url = Url.substr(0, index);
    }
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
    struct stat ResourceState;
    if (stat(("public" + DecodedUrl).c_str(), &ResourceState) < 0)
        throw HTTPError(500);
    if (S_ISREG(ResourceState.st_mode))
        this->dataPool.ResourceType = WB_FILE;
    else if (S_ISDIR(ResourceState.st_mode))
        this->dataPool.ResourceType = WB_DIRECTORY;
    else
        this->dataPool.ResourceType = WB_NEITHER;
}

int GetMethodId(std::string &Method)
{
    Lstring::tolower(Method);
    if (Method == "get")
        return (GET);
    else if ((Method == "post"))
        return (POST);
    else if ((Method == "delete"))
        return (DELETE);
    else
        return (OTHER);
}

void RequestParser::ParseHeaders(std::string data)
{
    std::vector<std::string> List;
    std::string MethodName;
    std::string TransferEncoding;
    std::string ContentLength;
    std::istringstream stream;

    List = Lstring::Split(data, "\r\n");
    stream.str((List.at(0)));
    stream >> MethodName >> this->dataPool.Url;

    /**
     *  TODO: check if URI valide
     */
    ParseUrl(this->dataPool.Url);
    DEBUGOUT(1, this->dataPool.Query);
    if (this->dataPool.Url.size() > 2048)
        throw HTTPError(414);

    this->dataPool.Method = GetMethodId(MethodName);

    for (std::vector<std::string>::iterator it = List.begin() + 1; it != List.end(); it++)
    {
        std::vector<std::string> Attr = Lstring::Split(*it, ": ");
        this->dataPool.Headers[Attr.at(0)] = Attr.at(1);
    }

    TransferEncoding = GetHeaderAttr(this->dataPool, "Transfer-Encoding");
    ContentLength = GetHeaderAttr(this->dataPool, "Content-Length");

    if (ContentLength.empty() && TransferEncoding.empty() && dataPool.Method == POST)
        throw HTTPError(501);

    if (!TransferEncoding.empty() && TransferEncoding != "chunked")
        throw HTTPError(501);
    if (dataPool.Method == POST)
    {
        if (TransferEncoding == "chunked")
            this->BodyReceiver = new ChunkController(dataPool);
        else if (!ContentLength.empty())
            this->BodyReceiver = new LenghtController(dataPool);

        this->BodyReceiver->SetRemaining(atoi(ContentLength.c_str()));
    }
    /**
     *  TODO: if => location have redirection like:  return 301 /home/index.html
     */
}

bool RequestParser::Parse(std::string data)
{
    size_t index = 0;
    this->buffer.append(data);
    if ((index = buffer.find("\r\n\r\n")) != std::string::npos && dataPool.Headers.empty())
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
            this->BodyReady = BodyReceiver ? BodyReceiver->Receiver(this->buffer) : false;
        if (this->BodyReady)
        {
            this->BodyReceiver->Parser();
            PrintfFullRequest();
            /**
             * TODO: if is a cgi respounce with output from cgi
             *         else responce with 201
             */
            return (dataPool.ResponseStatus = 201, true);
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

void RequestParser::PrintfFullRequest()
{
    DEBUGOUT(1, "\n\n-------------------------------\n\n");
    for (HeadersIterator i = dataPool.Headers.begin(); i != dataPool.Headers.end(); i++)
        DEBUGOUT(1, COLORED(i->first, Magenta) << " : " << COLORED(i->second, Green));
    // Lstring::LogAsBinary(this->dataPool.body, true);
}

std::string GetIndex(std::string &path)
{
    dirent *entry;
    DIR *dir;
    std::string IndexFileName;
    dir = opendir(path.c_str());
    if (dir == NULL)
        return ("");
    while ((entry = readdir(dir)) != NULL)
    {
        if (std::string(entry->d_name).find("index.") != std::string::npos)
        {
            IndexFileName = entry->d_name;
            return (closedir(dir), IndexFileName);
        }
    }
    return (closedir(dir), "");
}

void RequestParser::GetResourceFilePath()
{
    /**
     * TODO: get_auto_index() checks if config-ON
     * list all directory content in html
     */
    std::string ResourceFilePath;
    std::string IndexFileName;
    size_t LastDotPos;

    ResourceFilePath = "public" + dataPool.Url;
    if (access(ResourceFilePath.c_str(), F_OK) != 0)
        throw HTTPError(404);

    if (this->dataPool.ResourceType == WB_DIRECTORY)
    {
        if (*(this->dataPool.Url.end() - 1) != '/')
        {
            this->dataPool.Location = this->dataPool.Url + "/";
            throw HTTPError(301);
        }

        IndexFileName = GetIndex(ResourceFilePath);
        if (IndexFileName.empty())
        {
            /**
             * TODO: if auto indx on & method = Get
             * return autoindex of the directory
             */
            if (true)
                AutoIndex(this->dataPool, ResourceFilePath);
            else
                throw HTTPError(403);
        }
        else
            ResourceFilePath.append(IndexFileName);
    }
    DEBUGOUT(0, COLORED("ResourceFile Path ", Cyan) << COLORED(ResourceFilePath, Green));
    if ((LastDotPos = ResourceFilePath.find_last_of(".")) == std::string::npos)
        throw HTTPError(404);

    this->dataPool.File.Fd = open(ResourceFilePath.c_str(), O_RDONLY);
    this->dataPool.File.ResourceFileType = this->dataPool.Content_Types[ResourceFilePath.substr(LastDotPos)];
    if (this->dataPool.File.Fd < 0)
        throw HTTPError(404);
}

std::string GetHeaderAttr(DataPool &dataPool, std::string name)
{
    HeadersIterator it;

    it = dataPool.Headers.find(name);
    if (it != dataPool.Headers.end())
        return it->second;
    return ("");
}