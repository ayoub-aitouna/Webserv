#include "Include/GetRequest.hpp"

GetRequest::GetRequest(DataPool &dataPool) : Request(dataPool)
{
}

bool GetRequest::HandleRequest(std::string &data)
{
    (void)data;
    PrintfFullRequest();
    return (this->GetRequestedResource());
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
            if (this->dataPool.ServerConf->GetAutoindex())
                return AutoIndex(this->dataPool, ResourceFilePath), true;

            throw HTTPError(403);
        }
        else
            ResourceFilePath.append(IndexFileName);
    }

    FileExtention = GetFileExtention(ResourceFilePath);
    if (Containes(this->dataPool.ServerConf->GetCgi(), FileExtention))
    {
        return (Request::ExecuteCGI("/usr/bin/php-cgi", "GET"), false);
    }

    this->dataPool.File.Fd = IO::OpenFile(ResourceFilePath.c_str(), "r");
    this->dataPool.File.ResourceFileType = ConfigHandler::GetHttp().GetContentType(FileExtention);
    this->dataPool.ResponseStatus = OK;

    if (this->dataPool.File.Fd < 0)
        throw HTTPError(404);
    return true;
}

GetRequest::~GetRequest()
{
}