#include "Include/BodyController.hpp"

BodyController::BodyController(DataPool &dataPool) : dataPool(dataPool)
{
    this->isCGI = false;
    this->WrittenBytes = 0;
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
bool BodyController::GetIsCGI()
{
    return (this->isCGI);
}

void BodyController::CreateFile(std::string ContentType, bool isTemp)
{
    std::string extention;
    std::string uploadStore;

    uploadStore = this->dataPool.ServerConf->GetUpload_stor();
    extention = ConfigHandler::GetHttp().GetFileExtention(ContentType);
    extention = extention.empty() ? "" : "." + extention;
    if (ContentType.empty())
        ContentType = GetHeaderAttr(dataPool.Headers, "Content-Type");
    if (!isTemp)
        this->FileName = Lstring::RTrim(uploadStore, "/") + "/" + Lstring::RandomStr(10) + extention;
    else
        this->FileName = "/tmp/" + Lstring::RandomStr(16);
    if ((this->FileFd = IO::OpenFile(this->FileName.c_str(), "w+")) < 0)
        throw std::runtime_error("Couldn't Open File : " + this->FileName);
}

int BodyController::GetReadFd()
{
    try
    {
        int fd = IO::OpenFile(this->FileName.c_str(), "r");
        return (fd);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("OpenFile() failed At BodyController.cpp");
    }
}

int BodyController::GetWriteFd()
{
    return this->FileFd;
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

    if (!outputFile.is_open())
        throw std::runtime_error("Error While Opening File " + FileName);
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
            else if ((end = this->dataPool.body.find(EndBoundary)) != std::string::npos)
            {
                Part = this->dataPool.body.substr(0, end);
                this->dataPool.body.clear();
            }
            else
            {
                Part = this->dataPool.body;
                this->dataPool.body.clear();
            }
            WriteToFile(Part);
            if (this->dataPool.body.empty())
                break;
        }
    }
    else
        WriteToFile(this->dataPool.body);
}

void BodyController::WriteToFile(std::string &chunk)
{

    this->WrittenBytes += chunk.length();
    if (this->WrittenBytes > ConfigHandler::GetHttp().GetMaxBodySize())
        throw HTTPError(413);
    if (write(this->FileFd, chunk.c_str(), chunk.length()) < 0)
    {
        throw std::runtime_error("Write() Failed");
    }
    chunk.clear();
}

std::string &BodyController::GetFileName()
{
    return this->FileName;
}

BodyController::~BodyController()
{
}
