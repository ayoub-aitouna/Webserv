#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Proccesers.hpp"

class ResponseBuilder
{
public:
    ResponseBuilder(DataPool dataPool);
    ~ResponseBuilder();

private:
    std::string Buffer;
    DataPool dataPool;
    std::map<int, std::string> StatusCodes;

public:
    void FillBuffer();
    void FillHeaders(int StatusCode);
    int FlushBuffer(int SocketFd);
    void CreateStatusFile();
    void InitStatusCode();
    std::string GetDefaultErrorPagePath();
    std::map<int, std::string> &GetStatusCodes();
};
#endif