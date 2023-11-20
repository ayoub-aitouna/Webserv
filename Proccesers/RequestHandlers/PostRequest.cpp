#include "Include/PostRequest.hpp"

PostRequest::PostRequest(DataPool &dataPool) : Request(dataPool)
{
}

bool PostRequest::HandleRequest(std::string &data)
{
    bool SupportedUpload = false;
    PrintfFullRequest();

    if (!this->BodyReady)
        this->BodyReady = BodyReceiver ? BodyReceiver->Receiver(data) : false;
    if (this->BodyReady)
    {
        if (SupportedUpload)
            return (dataPool.ResponseStatus = CREATED, true);
        if (!GetRequestedResource())
            return false;
        this->BodyReceiver->Parser();

        /**
         * TODO: if is a cgi respounce with output from cgi
         *         else responce with 201
         */
        return (dataPool.ResponseStatus = CREATED, true);
    }
    return false;
}
int PostRequest::GetRequestedResource()
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
        if ((IndexFileName = GetIndex(ResourceFilePath)).empty())
            throw HTTPError(403);
        ResourceFilePath.append(IndexFileName);
    }

    DEBUGOUT(0, COLORED("POST - ResourceFile Path ", Blue) << COLORED(ResourceFilePath, Green));

    FileExtention = GetFileExtention(ResourceFilePath);

    /**
     * TODO: Files extention From Config File
     * Config Exutable of Cgi
     */
    if (FileExtention == ".php")
    {
        DEBUGOUT(1, COLORED("POST CGI " << this->ResourceFilePath, Cyan));
        Request::ExecuteCGI("/usr/bin/php-cgi", "POST");
        return false;
    }
    throw HTTPError(403);
    return true;
}

PostRequest::~PostRequest()
{
}