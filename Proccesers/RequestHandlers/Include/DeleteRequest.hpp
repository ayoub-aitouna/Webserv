#ifndef DELETEREQUEST_HPP
#define DELETEREQUEST_HPP

#include "Request.hpp"

class DeleteRequest : public Request
{
private:
    /* data */
public:
    DeleteRequest(DataPool &dataPool);
    virtual bool HandleRequest(std::string &data);
    virtual void GetRequestedResource();
    ~DeleteRequest();
};

#endif