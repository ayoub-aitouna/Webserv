#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <vector>
#include <map>
#include "EventHandlers/EventHandler.hpp"
#include "EventHandlers/HttpEventHandler.hpp"
#include "EventHandlers/AcceptEventHandler.hpp"
#include <unistd.h>
#include <poll.h>
#include <exception>
#include <stdexcept>
#include "Lib/Lstring.hpp"

class Reactor
{
public:
    Reactor();

private:
    std::vector<std::pair<int, EventHandler *> > clients;

public:
    void RegisterSocket(int SocketFd, EventHandler *);
    void UnRegisterSocket(int SocketFd);
    void HandleEvents();
    void Dispatch(struct pollfd *fds);
    void EventLoop();
};
typedef std::vector<std::pair<int, EventHandler *> >::iterator iterator;

#endif