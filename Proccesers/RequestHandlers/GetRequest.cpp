#include "Include/GetRequest.hpp"

GetRequest::GetRequest(DataPool &dataPool) : Request(dataPool)
{
}

bool GetRequest::HandleRequest(std::string &data)
{
    DEBUGOUT(1, "GET - HandleRequest RESOURCES");

    (void)data;
    PrintfFullRequest();
    this->Type = this->GetRequestedResource();
    if (Type == SCRIPT)
    {
        this->isCGIRuning = Request::ExecuteCGI("/usr/bin/php-cgi", "GET");
        this->dataPool.ResponseStatus = 200;
        return !this->isCGIRuning;
    }
    this->dataPool.ResponseStatus = 200;
    return (true);
}

int GetRequest::GetRequestedResource()
{
    Request::GetRequestedResource();

    std::string IndexFileName;
    std::string FileExtention;

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
            if (true)
            {
                AutoIndex(this->dataPool, ResourceFilePath);
                return NRMLFILE;
            }
            throw HTTPError(403);
        }
        else
            ResourceFilePath.append(IndexFileName);
    }

    DEBUGOUT(0, COLORED("GET - ResourceFile Path ", Green) << COLORED(ResourceFilePath, Green));

    FileExtention = GetFileExtention(ResourceFilePath);

    /**
     * TODO: Files extention From Config File
     * Config Exutable of Cgi
     */
    if (FileExtention == ".php")
        return SCRIPT;

    this->dataPool.File.Fd = IO::OpenFile(ResourceFilePath.c_str(), "r");
    this->dataPool.File.ResourceFileType = this->dataPool.Content_Types[FileExtention];
    if (this->dataPool.File.Fd < 0)
        throw HTTPError(404);
    return NRMLFILE;
}

GetRequest::~GetRequest()
{
}