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
    std::cout << "New Client" << std::endl;
    return new HttpEventHandler();
}