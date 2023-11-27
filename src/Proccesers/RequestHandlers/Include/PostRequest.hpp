#ifndef POSTREQUEST_HPP
#define POSTREQUEST_HPP
#include "Request.hpp"

enum
{
    ZERO,
    UP_INPROGRESS,
    CGI_INPROGRESS,
    DONE
};

class PostRequest : public Request
{
private:
    int fds[2];
    int UploadBodyState;
    std::string FileExtention;

public:
    PostRequest(DataPool &dataPool);
    virtual bool HandleRequest(std::string &data);
    virtual int GetRequestedResource();
    bool RecieveBody(std::string &data);
    ~PostRequest();
};

#endif