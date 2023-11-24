#include "Include/PostRequest.hpp"

PostRequest::PostRequest(DataPool &dataPool) : Request(dataPool)
{
    this->SupportedUpload = true;
    this->UploadBodyState = ZERO;
}

bool PostRequest::HandleRequest(std::string &data)
{
    if (this->UploadBodyState == ZERO && GetRequestedResource())
    {
        PrintfFullRequest();
        this->BodyReceiver->Parser();
        return (dataPool.ResponseStatus = CREATED, true);
    }
    if (this->UploadBodyState == UP_INPROGRESS || this->UploadBodyState == CGI_INPROGRESS)
    {
        if (!this->BodyReady)
            this->BodyReady = BodyReceiver ? BodyReceiver->Receiver(data) : false;
        if (this->BodyReady)
        {
            if (this->UploadBodyState == UP_INPROGRESS)
                return (this->UploadBodyState = DONE,
                        dataPool.ResponseStatus = CREATED, true);
            else if (this->UploadBodyState == CGI_INPROGRESS)
                return (this->UploadBodyState = DONE, false);
        }
    }
    return false;
}
int PostRequest::GetRequestedResource()
{
    Request::GetRequestedResource();
    std::string IndexFileName;

    if (this->dataPool.ResourceType == WB_DIRECTORY)
    {
        if (*(this->dataPool.Url.end() - 1) != '/')
        {
            this->dataPool.Location = this->dataPool.Url + "/";
            throw HTTPError(301);
        }
        if ((IndexFileName = GetIndex(ResourceFilePath)).empty() && !SupportedUpload)
            throw HTTPError(403);
        ResourceFilePath.append(IndexFileName);
    }
    FileExtention = GetFileExtention(ResourceFilePath);

    /**
     * TODO: Files extention From Config File
     * Config Exutable of Cgi
     */
    if (FileExtention == "php")
    {
        /**
         * TODO: Files extention From Config File
         * Config Exutable of Cgi
         */
        if (pipe(fds) < 0)
            ServerError("pipe() failed");
        this->BodyReceiver->SetFileFd(fds[0], fds[1]);
        this->BodyReceiver->SetIsCGI(true);
        Request::ExecuteCGI("/usr/bin/php-cgi", "POST");
        return (this->UploadBodyState = CGI_INPROGRESS, false);
    }
    if (this->SupportedUpload)
    {
        DEBUGOUT(1, "HERERERER");
        this->BodyReceiver->SetIsCGI(false);
        if (GetHeaderAttr(this->dataPool, "Content-Type").find("boundary=") == std::string::npos)
            this->BodyReceiver->CreateFile();
        return (this->UploadBodyState = UP_INPROGRESS, false);
    }
    throw HTTPError(403);
}

PostRequest::~PostRequest()
{
}