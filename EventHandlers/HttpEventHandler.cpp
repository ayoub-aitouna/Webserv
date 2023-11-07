#include "HttpEventHandler.hpp"
#include <assert.h>

HttpEventHandler::HttpEventHandler(int SocketFd, struct sockaddr_storage address, socklen_t address_len) : EventHandler(SocketFd), request(""), address(address), address_len(address_len)
{
}

HttpEventHandler::HttpEventHandler() : EventHandler(-1)
{
}

int HttpEventHandler::Read()
{
    return (-1);
}

int HttpEventHandler::Write()
{
    return (-1);
}

EventHandler *HttpEventHandler::Accept(void)
{
    return (NULL);
}

const int &HttpEventHandler::GetSocketFd() const
{
    return this->SocketFd;
}

 HttpEventHandler::~HttpEventHandler()
{
}
