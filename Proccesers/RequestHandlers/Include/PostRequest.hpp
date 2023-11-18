#ifndef POSTREQUEST_HPP
#define POSTREQUEST_HPP
#include "Request.hpp"

class PostRequest : public Request
{
private:
    /* data */
public:
    PostRequest(DataPool &dataPool);
    virtual bool HandleRequest(std::string &data);
    virtual void GetRequestedResource();
    ~PostRequest();
};

#endif