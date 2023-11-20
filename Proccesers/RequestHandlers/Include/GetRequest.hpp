#ifndef GETREQUEST_HPP
#define GETREQUEST_HPP
#include "Request.hpp"

class GetRequest : public Request
{
private:
    /* data */
public:
    GetRequest(DataPool &dataPool);
    virtual bool HandleRequest(std::string &data);
    virtual int GetRequestedResource();
    ~GetRequest();
};

#endif