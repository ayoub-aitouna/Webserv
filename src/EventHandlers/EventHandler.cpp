
#include "EventHandler.hpp"

EventHandler::EventHandler(int SocketFd) : SocketFd(SocketFd)
{
}

const int &EventHandler::GetSocketFd() const
{
    return this->SocketFd;
}

EventHandler::~EventHandler()
{
}