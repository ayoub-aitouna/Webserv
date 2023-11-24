#ifndef DELETEREQUEST_HPP
#define DELETEREQUEST_HPP

#include "Request.hpp"

class DeleteRequest : public Request
{

public:
    DeleteRequest(DataPool &dataPool);
    virtual bool HandleRequest(std::string &data);
    virtual int GetRequestedResource();
    void DeleteDirectory(std::string Dir);
    ~DeleteRequest();
};

#endif