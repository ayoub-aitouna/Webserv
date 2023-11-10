#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "../Lib/Lstring.hpp"
#include "../Lib/MimeTypes.hpp"
#include "Request.hpp"
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include <iostream>

#define KB 1024

class Response
{
public:
    Response(Request request);
    ~Response();

private:
    std::string Buffer;

    Request request;

    std::map<std::string, std::string> Content_Types;
    int File;
    std::string FileType;

public:
    void FillBuffer();
    void GetFilePath();
    void FillHeaders(int StatusCode);
    int FlushBuffer(int SocketFd);
};
#endif