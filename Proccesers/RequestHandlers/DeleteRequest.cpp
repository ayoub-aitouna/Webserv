#include "Include/DeleteRequest.hpp"

DeleteRequest::DeleteRequest(DataPool &dataPool) : Request(dataPool)
{
}

bool DeleteRequest::HandleRequest(std::string &data)
{
    (void)data;
    return false;
}

void DeleteRequest::GetRequestedResource()
{
    Request::GetRequestedResource();

    std::string IndexFileName;
    std::string FileExtention;

    if (this->dataPool.ResourceType == WB_DIRECTORY)
    {
        if (*(this->dataPool.Url.end() - 1) != '/')
            throw HTTPError(409);
        if ((IndexFileName = GetIndex(ResourceFilePath)).empty())
            throw HTTPError(403);
        ResourceFilePath.append(IndexFileName);
    }

    DEBUGOUT(0, COLORED("DELETE - ResourceFile Path ", Red) << COLORED(ResourceFilePath, Green));

    FileExtention = GetFileExtention(ResourceFilePath);

    /**
     * TODO: Files extention From Config File
     * Config Exutable of Cgi
     */
    if (FileExtention == ".php" || FileExtention == ".py")
    {
        return;
    }
}

DeleteRequest::~DeleteRequest()
{
}