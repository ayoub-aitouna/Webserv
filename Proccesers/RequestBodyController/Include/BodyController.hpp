#ifndef BODYCONTROLLER_HPP
#define BODYCONTROLLER_HPP

#include "../../Include/Proccesers.hpp"
#include "../../../HTTPError/HTTPError.hpp"

class BodyController
{
public:
    BodyController(DataPool &dataPool);
    virtual int Receiver(std::string &data) = 0;
    virtual void SetRemaining(unsigned long Remaining) = 0;
    void Parser();
    WBSRVFILE SaveMultiPartFile(std::string &part);
    void SaveBodyAsFile();
    std::string &GetFileName();
    void CreateFile(std::string ContentType = "");
    void SetFileFd(int ReadFd, int WriteFd);
    int GetReadFd();
    int GetWriteFd();
    void SetIsCGI(bool isCGI);
    virtual ~BodyController();

protected:
    unsigned long Remaining;
    int Encoding;
    DataPool &dataPool;
    int BodyFileFds[2];
    std::string FileName;
    bool isCGI;
    // multipart/form-data;
    std::string Boundary;
    std::string DivaderBoundary;
    std::string EndBoundary;
};

#endif