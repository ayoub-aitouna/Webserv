#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "../Lib/Lstring.hpp"
#include "../Lib/MimeTypes.hpp"
#include "RequestParser.hpp"
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include <iostream>

#define KB 1024
#define SSTR(x) static_cast<std::ostringstream &>(std::ostringstream() << std::dec << x).str()

class ResponseBuilder
{
public:
    ResponseBuilder(Medium medium);
    ~ResponseBuilder();

private:
    std::string Buffer;
    Medium medium;
    std::map<int, std::string> StatusCodes;

public:
    void FillBuffer();
    void FillHeaders(int StatusCode);
    int FlushBuffer(int SocketFd);
    void InitStatusCode();
};
#endif