#include "Include/PostRequest.hpp"

#define DEBUG_GETREQUEST 1

PostRequest::PostRequest(DataPool &dataPool) : Request(dataPool)
{
    this->UploadBodyState = ZERO;
}

bool PostRequest::HandleRequest(std::string &data)
{
    if (this->UploadBodyState == ZERO && GetRequestedResource())
        return (PrintfFullRequest(), dataPool.ResponseStatus = OK, true);
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
                return (Execute(ResourceFilePath, "POST"),
                        this->UploadBodyState = DONE, false);
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
        if ((IndexFileName = GetIndex(ResourceFilePath)).empty() &&
            !this->dataPool.ServerConf->GetUpload())
            throw HTTPError(403);
        ResourceFilePath.append(IndexFileName);
    }

    FileExtention = GetFileExtention(ResourceFilePath);
    if (Containes(this->dataPool.ServerConf->GetCgi(), FileExtention))
    {
        this->BodyReceiver->CreateFile("", true);
        this->BodyReceiver->SetIsCGI(true);
        return (this->UploadBodyState = CGI_INPROGRESS, false);
    }
    if (this->dataPool.ServerConf->GetUpload())
    {
        if (!this->BodyReceiver)
            throw HTTPError(403);
        this->BodyReceiver->SetIsCGI(false);
        if (GetHeaderAttr(dataPool.Headers, "Content-Type").find("boundary=") == std::string::npos)
            this->BodyReceiver->CreateFile();
        return (this->UploadBodyState = UP_INPROGRESS, false);
    }
    throw HTTPError(403);
}

PostRequest::~PostRequest()
{
}