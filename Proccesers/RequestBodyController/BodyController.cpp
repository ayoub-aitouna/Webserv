#include "Include/BodyController.hpp"

BodyController::BodyController(DataPool &dataPool) : dataPool(dataPool)
{
    std::string extention;
    HeadersIterator it;

    this->Remaining = 0;
    this->Encoding = NON;
    it = dataPool.Reverse_Content_Types.find(GetHeaderAttr(this->dataPool, "Content-Type"));
    if (it != dataPool.Reverse_Content_Types.end())
        extention = it->second;
    this->FileName = "public/" + Lstring::RandomStr(10) + extention;
    int fd = IO::OpenFile(this->FileName.c_str(), "w+");
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
    unlink(FileName.c_str());
}

void BodyController::Parser()
{
    DEBUGOUT(1, "STARTED PARSING THE BODY   ");
    std::string Boundary;
    size_t index, start, end;

    std::string ContentType;
    std::vector<std::string> Parts;

    Boundary = "boundary=";
    ContentType = GetHeaderAttr(this->dataPool, "Content-Type");
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

WBSRVFILE BodyController::SaveBodyAsFile()
{
    DEBUGOUT(0, "SAVE CHUNK AS FILE   " << this->dataPool.body.size());
    WBSRVFILE File;
    int i;
    if ((i = write(this->BodyFileFds[1], this->dataPool.body.c_str(), this->dataPool.body.length())) < 0)
    {
        DEBUGOUT(1, COLORED("Error While write into File : SaveBodyAsFile() "
                                << this->BodyFileFds[1]
                                << "\nData : {\n"
                                << this->dataPool.body
                                << "\n}\n"
                                << "Data Size " << this->dataPool.body.size()
                                << "\n write() return : " << i,
                            Blue));
        throw HTTPError(500);
    }
    this->dataPool.body.clear();
    return (File);
}

std::string &BodyController::GetFileName()
{
    return this->FileName;
}

BodyController::~BodyController()
{
}
