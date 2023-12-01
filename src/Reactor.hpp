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
#include <sys/epoll.h>

class Reactor
{
public:
    Reactor();
    ~Reactor();

private:
    std::map<int, EventHandler *> clients;
    struct epoll_event *events;
    int epoll_fd;
    size_t MAX_EVENTS;
    int event_count;

public:
    void RegisterSocket(int SocketFd, EventHandler *);
    void UnRegisterSocket(int SocketFd);
    void HandleEvents();
    void Dispatch();
    void EventLoop();
};
typedef std::map<int, EventHandler *>::iterator iterator;
void CheckCGIOutput(HttpEventHandler *client);
#endif