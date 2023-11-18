#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../../RequestBodyController/Include/BodyController.hpp"
#include "../../RequestBodyController/Include/LenghtController.hpp"
#include "../../RequestBodyController/Include/ChunkController.hpp"
#include "../../Include/AutoIndex.hpp"

enum
{
    OK = 200,
    CREATED = 201,
    NOTFOUND = 404,
    FORBIDDEN = 403
};

class Request
{
public:
    DataPool &dataPool;
    BodyController *BodyReceiver;
    std::string ResourceFilePath;
    bool BodyReady;

public:
    Request(DataPool &dataPool);
    virtual bool HandleRequest(std::string &data) = 0;
    virtual void GetRequestedResource();
    void PrintfFullRequest();
    std::string GetIndex(std::string &path);
    std::string GetFileExtention(std::string &FilePath);
    void SetBodyController(int Type, u_int64_t Remaining);
    ~Request();
};

#endif