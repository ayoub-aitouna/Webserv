#include "Reactor.hpp"

Reactor::Reactor()
{
}

void Reactor::RegisterSocket(int socketFd, EventHandler *eventHandler)
{
    this->clients.push_back(std::make_pair(socketFd, eventHandler));
}

void Reactor::UnRegisterSocket(int SocketFd)
{
    std::vector<std::pair<int, EventHandler *>>::iterator it;
    for (it = this->clients.begin(); it != this->clients.end(); it++)
    {
        if (it->first == SocketFd)
        {
            close(it->first);
            delete it->second;
            this->clients.erase(it);
            break;
        }
    }
}

struct pollfd *Reactor::IOMultiplexing()
{
    struct pollfd *fds = (struct pollfd *)malloc(this->clients.size() * sizeof(struct pollfd));
    std::vector<std::pair<int, EventHandler *>>::iterator it;
    int i = 0;
    for (it = this->clients.begin(); it != this->clients.end(); it++)
    {
        fds[i].fd = it->first;
        fds[i].events = POLLIN | POLLOUT;
        i++;
    }
    if (poll(fds, i, 3000) < 0)
        throw std::runtime_error("poll() failled");
    return fds;
}

void Reactor::EventLoop()
{
    while (true)
    {
        struct pollfd *fds = IOMultiplexing();
        for (std::vector<std::pair<int, EventHandler *>>::iterator it = this->clients.begin(); it != this->clients.end(); it++)
        {
            if (it->first & POLLIN)
                Dispatch();
        }
    }
}
