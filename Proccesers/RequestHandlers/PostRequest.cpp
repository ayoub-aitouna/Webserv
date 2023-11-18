#include "Include/PostRequest.hpp"

PostRequest::PostRequest(DataPool &dataPool) : Request(dataPool)
{
}

bool PostRequest::HandleRequest(std::string &data)
{
    if (!this->BodyReady)
        this->BodyReady = BodyReceiver ? BodyReceiver->Receiver(data) : false;
    if (this->BodyReady)
    {
        this->BodyReceiver->Parser();
        PrintfFullRequest();
        GetRequestedResource();
        /**
         * TODO: if is a cgi respounce with output from cgi
         *         else responce with 201
         */
        return (dataPool.ResponseStatus = CREATED, true);
    }
    return false;
}

void PostRequest::GetRequestedResource()
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
        if (!(IndexFileName = GetIndex(ResourceFilePath)).empty())
            ResourceFilePath.append(IndexFileName);
        throw HTTPError(403);
    }

    DEBUGOUT(0, COLORED("POST - ResourceFile Path ", Blue) << COLORED(ResourceFilePath, Green));

    FileExtention = GetFileExtention(ResourceFilePath);

    /**
     * TODO: Files extention From Config File
     * Config Exutable of Cgi
     */
    if (FileExtention == ".php" || FileExtention == ".py")
    {
        //   RunCgi(ResourceFilePath);
        return;
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
            throw HTTPError(500);
        throw HTTPError(204);
    }
}

PostRequest::~PostRequest()
{
}