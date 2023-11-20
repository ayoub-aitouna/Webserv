#include "Include/DeleteRequest.hpp"

DeleteRequest::DeleteRequest(DataPool &dataPool) : Request(dataPool)
{
}

bool DeleteRequest::HandleRequest(std::string &data)
{
    (void)data;
    return false;
}

int DeleteRequest::GetRequestedResource()
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
        return false;
    }

    if (this->dataPool.ResourceType == WB_DIRECTORY)
    {
        /**
         * TODO:
         * Recursively Delete All Files And Folders in Requested Directory
         */
    }
    else if (this->dataPool.ResourceType == WB_FILE)
    {
        if (unlink(ResourceFilePath.c_str()) < 0)
        {
            DEBUGOUT(1, COLORED("Error While deleting File " << ResourceFilePath, Red));

            throw HTTPError(500);
        }
        throw HTTPError(204);
    }
    return true;
}

DeleteRequest::~DeleteRequest()
{
}