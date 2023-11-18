#include "Include/BodyController.hpp"

BodyController::BodyController(DataPool &dataPool) : dataPool(dataPool)
{
    this->Remaining = 0;
    this->Encoding = NON;
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
    SaveBodyAsFile();
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
    DEBUGOUT(1, "SAVE AS FILE   " << this->dataPool.body.size());

    std::string extention;
    HeadersIterator it;
    std::string FileName;
    WBSRVFILE File;

    it = dataPool.Reverse_Content_Types.find(GetHeaderAttr(this->dataPool, "Content-Type"));
    if (it != dataPool.Reverse_Content_Types.end())
        extention = it->second;
    FileName = "public/" + Lstring::RandomStr(10) + extention;
    std::ofstream OutputFile(FileName.c_str(), std::ios::binary);
    if (!OutputFile)
    {
        DEBUGOUT(1, "Couldn't Open File : " << FileName);
        throw HTTPError(500);
    }
    OutputFile << this->dataPool.body;
    return (File);
}

BodyController::~BodyController()
{
}
