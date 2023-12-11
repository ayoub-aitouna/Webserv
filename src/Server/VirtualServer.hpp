
#ifndef VIRTUALSERVER_HPP
#define VIRTUALSERVER_HPP

#include "../Reactor.hpp"
#include "string"

class VirtualServer
{

public:
    VirtualServer(int SocketFd, bool SSlOn, std::string cert = "", std::string Key = "");
    SSL_CTX *getSLL_CTX();
    bool IsSSlOn();
    int GetSocketFd();
    ~VirtualServer();

private:
    int socketFd;
    bool SSlOn;
    SSL_CTX *ctx;
};

#endif