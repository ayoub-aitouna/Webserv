#ifndef BODYCONTROLLER_HPP
#define BODYCONTROLLER_HPP

#include "../../Include/Core.hpp"
#include "../../../HTTPError/HTTPError.hpp"

class BodyController
{
public:
    BodyController(DataPool &dataPool);
    virtual int Receiver(std::string &data) = 0;
    void Parser();
    WBSRVFILE SaveMultiPartFile(std::string &part);
    void SaveBodyAsFile();
    std::string &GetFileName();
    void CreateFile(std::string ContentType = "", bool isTemp = false);
    // //void SetFileFd(int ReadFd, int WriteFd);
    int GetReadFd();
    int GetWriteFd();
    void WriteToFile(std::string &chunk);
    void SetIsCGI(bool isCGI);
    bool GetIsCGI();
    virtual ~BodyController();

protected:
    unsigned long long Remaining;
    unsigned long long WrittenBytes;
    int Encoding;
    DataPool &dataPool;
    int FileFd;
    std::string FileName;
    bool isCGI;
    // multipart/form-data;
    std::string Boundary;
    std::string DivaderBoundary;
    std::string EndBoundary;
};

#endif