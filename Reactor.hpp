#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <vector>
#include <map>
#include "EventHandlers/EventHandler.hpp"
#include "EventHandlers/HttpEventHandler.hpp"
#include <unistd.h>
#include <poll.h>

class Reactor
{
public:
    Reactor();

private:
    std::vector<std::pair<int, EventHandler *>> clients;

public:
    void RegisterSocket(int SocketFd, EventHandler *);
    void UnRegisterSocket(int SocketFd);
    struct pollfd *IOMultiplexing();
    void Dispatch();
    void EventLoop();
};

#endif