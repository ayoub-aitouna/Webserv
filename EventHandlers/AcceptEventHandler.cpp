#include "AcceptEventHandler.hpp"

AcceptEventHandler::AcceptEventHandler(int SocketFd) : EventHandler(SocketFd)
{
}

int AcceptEventHandler::Read()
{
    return -1;
}

int AcceptEventHandler::Write()
{
    return -1;
}

EventHandler *AcceptEventHandler::Accept(void)
{
    struct sockaddr_storage addr;
    socklen_t addrlen;
    int socket;

    addrlen = sizeof(addrlen);
    socket = accept(this->SocketFd, (struct sockaddr *)&addr, &addrlen);
    if (socket < 0)
        throw std::runtime_error("accept() failed");
    return (new HttpEventHandler(socket, addr, addrlen));
}