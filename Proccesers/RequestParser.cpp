#include "RequestParser.hpp"

RequestParser::RequestParser()
{
    this->Content_Types = MimeTypes::GetContenTypes();
    this->Reverse_Content_Types = MimeTypes::GetReverseContenTypes(Content_Types);
    this->Encoding = NON;
    this->Remaining = 0;
    this->BodyReady = false;
    this->dataPool.Method = OTHER;
    this->dataPool.File.Fd = NOBODY;
    this->datasize = 0;
    ChunkState = Size;
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

void RequestParser::ParseHeaders(std::string data)
{
    std::vector<std::string> List;
    std::string strMethod;
    List = Lstring::Split(data, "\r\n");
    std::istringstream stream(List.at(0));

    stream >> strMethod >> this->dataPool.Url;
    ParseUrl(this->dataPool.Url);
    DEBUGOUT(1, this->dataPool.Query);
    Lstring::tolower(strMethod);
    this->dataPool.Method = strMethod == "get" ? GET : (strMethod == "post") ? POST
                                                   : (strMethod == "delete") ? DELETE
                                                                             : OTHER;

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
    if (this->dataPool.Url.size() > 2048)
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
            return (true);
    }

    if (Encoding == Chunked)
    {
        size_t index;
        size_t Part;
        std::string RawData;

        while (!data.empty())
        {
            switch (ChunkState)
            {
            case Size:

                if ((index = data.find("\r\n")) == std::string::npos)
                {
                    if (data.size() < 7)
                        return false;
                    DEBUGOUT(0, COLORED("Size Error !!!!!!! ", Red)
                                    << "\ndata.size  " << data.size()
                                    << "\n---------------------\n"
                                    << COLORED(" Data ERROR !!!  " << Lstring::base64_encode(data), Red)
                                    << COLORED(" Start Data !!!  " << Lstring::base64_encode(this->dataPool.body.substr(0, 10)), Blue)
                                    << "\n---------------------\n"
                                    << COLORED(" End Data !!!    " << Lstring::base64_encode(this->dataPool.body.substr(dataPool.body.size() - 10, dataPool.body.size())), Blue));

                    ChunkState = Error;
                    break;
                }
                Remaining = strtol(data.c_str(), 0, 16);

                DEBUGOUT(0, COLORED("Start Of uffer " << Lstring::base64_encode(data.substr(0, 10)), Magenta)
                                << "Remaining  " << Remaining
                                << " index  " << index);
                ChunkState = Remaining != 0 ? Data : End;
                data = data.erase(0, index + 2);
                break;
            case Data:
                if (Remaining != 0)
                {
                    Part = ((size_t)Remaining) > data.size() ? data.size() : Remaining;
                    RawData = data.substr(0, Part);
                    dataPool.body.append(RawData);
                    Remaining -= Part;
                    data = data.erase(0, Part);
                }
                if (data.size() < 2)
                    return false;
                if (Remaining == 0)
                {
                    if ((data.size() > 2) && (data.at(0) != '\r' || data.at(1) != '\n'))
                    {
                        DEBUGOUT(0, COLORED("Data Error !!!!!!! ", Red)
                                        << "  Part  " << Part << "\n"
                                        << "  Remaining  " << Remaining << "\n"
                                        << COLORED(" Raw Data !!!  " << Lstring::base64_encode(RawData), Green)
                                        << "\n---------------------\n"
                                        << COLORED(" Data ERROR !!!  " << Lstring::base64_encode(data), Red)
                                        << COLORED(" Start Data !!!  " << Lstring::base64_encode(this->dataPool.body.substr(0, 10)), Blue)
                                        << "\n---------------------\n"
                                        << COLORED(" End Data !!!    " << Lstring::base64_encode(this->dataPool.body.substr(dataPool.body.size() - 10, dataPool.body.size())), Blue));

                        ChunkState = Error;
                    }
                    else
                    {
                        data = data.erase(0, 2);
                        ChunkState = Size;
                    }
                }
                break;
            case End:
                return (true);
                DEBUGOUT(1, COLORED("END !!!  " << Lstring::base64_encode(data), Green));
                break;

            default:
                break;
            }
            if (ChunkState == Error)
            {
                DEBUGOUT(1, COLORED("ERROR !!!", Red));
                exit(0);
            }
            /**
             *  TODO: if => RequestParser body larger than client max body size in config file
             *        change <1024> with max-body from config file */
            // if (dataPool.body.size() > 90 * 1024 * 1024)
            //     throw HTTPError(413);
        }
    }

    if (Encoding == NON)
        return (true);

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

    Boundary = "boundary=";
    ContentType = GetHeaderAttr("Content-Type");
    if (ContentType.empty())
        return;
    if ((index = ContentType.find(Boundary)) != std::string::npos)
    {
        /**
         * TODO: parse multipart/form-data body
         */
        Boundary = "--" + ContentType.substr(index + Boundary.size());

        for (;;)
        {
            if ((start = this->dataPool.body.find(Boundary)) == std::string::npos)
                break;
            this->dataPool.body = this->dataPool.body.substr(start + Boundary.size());
            if ((end = this->dataPool.body.find(Boundary)) == std::string::npos)
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
    DEBUGOUT(1, "\n\n-------------------------------\n\n");
    for (HeadersIterator i = dataPool.Headers.begin(); i != dataPool.Headers.end(); i++)
        DEBUGOUT(1, COLORED(i->first, Magenta) << " : " << COLORED(i->second, Green));
    // Lstring::LogAsBinary(this->dataPool.body, true);
}

void RequestParser::AutoIndex(std::string &dirPath)
{
    dirent *entry;
    std::string buffer;
    DIR *dir;
    std::string FileName = "/tmp/" + Lstring::RandomStr(16);

    dir = opendir(dirPath.c_str());
    if (dir == NULL)
        return;
    buffer = "<!DOCTYPE html>"
             "<html lang=\"en\">"
             "<head>"
             "<meta charset=\"UTF-8\">"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
             "<title>List Files</title>"
             "</head>"
             "<body>"
             "<ul>";
    while ((entry = readdir(dir)) != NULL)
    {
        std::ostringstream ss;
        ss << "<li> <a href=\"" << entry->d_name << "\">" << entry->d_name << "</a></li>";
        buffer.append(ss.str());
    }
    buffer.append("</ul></body></html>");
    this->dataPool.File.Fd = IO::OpenFile(FileName.c_str(), "w+");

    write(this->dataPool.File.Fd, buffer.c_str(), buffer.size());
    close(this->dataPool.File.Fd);
    this->dataPool.File.Fd = IO::OpenFile(FileName.c_str(), "r+");
    this->dataPool.File.ResourceFileType = "text/html";
    closedir(dir);
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
                AutoIndex(ResourceFilePath);
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
    this->dataPool.File.ResourceFileType = this->Content_Types[ResourceFilePath.substr(LastDotPos)];
    if (this->dataPool.File.Fd < 0)
        throw HTTPError(404);
}
