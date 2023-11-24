#include "Include/BodyController.hpp"

BodyController::BodyController(DataPool &dataPool) : dataPool(dataPool)
{
    this->isCGI = false;
    std::string ContentType;
    std::string BoundaryName = "boundary=";
    size_t index;

    if ((index = (ContentType = GetHeaderAttr(dataPool.Headers, "Content-Type"))
                     .find(BoundaryName)) != std::string::npos)
    {
        this->Boundary = "--" + ContentType.substr(index + BoundaryName.size());
        this->DivaderBoundary = Boundary + '\r' + '\n';
        this->EndBoundary = Boundary + '-' + '-';
    }
    this->Remaining = 0;
    this->Encoding = NON;
}

void BodyController::SetIsCGI(bool isCGI)
{
    this->isCGI = isCGI;
}

void BodyController::CreateFile(std::string ContentType)
{
    std::string extention;
    HeadersIterator it;
    int fd;

    if (ContentType.empty())
        ContentType = GetHeaderAttr(dataPool.Headers, "Content-Type");
    this->FileName = this->dataPool.ServerConf->GetUpload_stor() + Lstring::RandomStr(10) +
                     ConfigHandler::GetHttp().GetFileExtention(ContentType);
    DEBUGOUT(1, "NAME " << this->FileName);
    fd = IO::OpenFile(this->FileName.c_str(), "w+");
    if (fd < 0)
    {
        DEBUGOUT(1, "Couldn't Open File : " << this->FileName);
        throw HTTPError(500);
    }
    SetFileFd(fd, fd);
}

int BodyController::GetReadFd()
{
    return this->BodyFileFds[0];
}

int BodyController::GetWriteFd()
{
    return this->BodyFileFds[1];
}

void BodyController::SetFileFd(int ReadFd, int WriteFd)
{
    this->BodyFileFds[0] = ReadFd;
    this->BodyFileFds[1] = WriteFd;
}

void BodyController::Parser()
{
    std::string Boundary;
    size_t index, start, end;

    std::string ContentType;
    std::vector<std::string> Parts;

    Boundary = "boundary=";
    ContentType = GetHeaderAttr(dataPool.Headers, "Content-Type");
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
}

WBSRVFILE BodyController::SaveMultiPartFile(std::string &part)
{
    std::string FileName;
    std::string tmp;
    std::string Content_Type;
    WBSRVFILE File;

    FileName = Lstring::RandomStr(10);
    tmp = Lstring::ExtractFromString(part, "filename=\"", "\"");
    if (!tmp.empty())
        FileName = tmp;

    Content_Type = Lstring::ExtractFromString(part, "Content-Type: ", CRLF);

    part = part.substr(part.find(DBLCRLF) + 4);
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

void BodyController::SaveBodyAsFile()
{
    if (!Boundary.empty() && !this->isCGI)
    {
        size_t start, end;
        std::string Part;
        for (;;)
        {
            if ((start = this->dataPool.body.find(DivaderBoundary)) != std::string::npos)
            {
                if ((end = this->dataPool.body.find("\r\n\r\n", start + DivaderBoundary.size())) != std::string::npos)
                {
                    this->CreateFile(Lstring::ExtractFromString(this->dataPool.body, "Content-Type: ", CRLF, "text/plain"));
                    this->dataPool.body = this->dataPool.body.substr(end + 4);
                }
                else
                    break;
            }
            if ((end = this->dataPool.body.find(DivaderBoundary)) != std::string::npos)
            {
                Part = this->dataPool.body.substr(0, end);
                if (this->dataPool.body.find(EndBoundary, end) != std::string::npos)
                    this->dataPool.body.clear();
                else
                    this->dataPool.body = this->dataPool.body.substr(end);
            }
            else
            {
                Part = this->dataPool.body;
                this->dataPool.body.clear();
            }
            if (write(BodyFileFds[1], Part.c_str(), Part.length()) < 0)
                throw HTTPError(500);
            if (this->dataPool.body.empty())
                break;
        }
    }
    else
    {
        DEBUGOUT(0, "WRITNG USING SAVE FIILE CODE");
        if (write(BodyFileFds[1], dataPool.body.c_str(), dataPool.body.length()) < 0)
            throw HTTPError(500);

        this->dataPool.body.clear();
    }
}

std::string &BodyController::GetFileName()
{
    return this->FileName;
}

BodyController::~BodyController()
{
}
