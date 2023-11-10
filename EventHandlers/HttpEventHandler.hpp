#ifndef READEVENTHANDLER_HPP
#define READEVENTHANDLER_HPP
#include <vector>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "EventHandler.hpp"
#include "../Proccesers/Response.hpp"
#include "../Proccesers/Request.hpp"
#include <sstream>

class Client
{
public:
    std::string data;
    struct sockaddr_storage address;
    socklen_t address_len;
};

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
    Client client;
    Request request;
    Response *response;
};

#endif