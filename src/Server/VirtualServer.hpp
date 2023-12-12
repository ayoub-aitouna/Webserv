
#ifndef VIRTUALSERVER_HPP
#define VIRTUALSERVER_HPP

#include "../Reactor.hpp"
#include "string"
#include <openssl/ssl.h>

class VirtualServer
{

public:
    VirtualServer(int SocketFd, bool SSlOn, std::string cert = "", std::string Key = "");
    void *getSLL_CTX();
    bool IsSSlOn();
    int GetSocketFd();
    ~VirtualServer();

private:
    int socketFd;
    bool SSlOn;
    void *ctx;
};

#endif