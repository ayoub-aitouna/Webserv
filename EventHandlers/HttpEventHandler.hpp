#ifndef READEVENTHANDLER_HPP
#define READEVENTHANDLER_HPP
#include <vector>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "EventHandler.hpp"

class HttpEventHandler : public EventHandler
{
public:
    HttpEventHandler(int SocketFd, struct sockaddr_storage address, socklen_t address_len);
    HttpEventHandler();
    virtual ~HttpEventHandler();

public:
    int Read();
    int Write();
    EventHandler *Accept();
    const int &GetSocketFd() const;

private:
    std::string request;
    struct sockaddr_storage address;
    socklen_t address_len;
};

#endif