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

void Reactor::HandleEvents()
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
    if (poll(fds, i, ) >= 0)
        Dispatch(fds);
    else
        throw std::runtime_error("poll() failled");
}

void Reactor::Dispatch(struct pollfd *fds)
{
    int i = 0;
    for (std::vector<std::pair<int, EventHandler *>>::iterator it = this->clients.begin(); it != this->clients.end(); it++)
    {
        if (fds[i].events & (POLLERR | POLLRDNORM))
        {
            AcceptEventHandler *server;
            HttpEventHandler *client;

            if ((server = dynamic_cast<AcceptEventHandler *>(it->second)) != NULL)
                server->Accept();
            else if ((client = dynamic_cast<HttpEventHandler *>(it->second)) != NULL)
                client->Read();
        }
        if (fds[i].events & (POLLERR | POLLWRNORM))
        {
            if (dynamic_cast<HttpEventHandler *>(it->second)->Write() == 0)
                UnRegisterSocket(it->first);
        }
        i++;
    }
}

void Reactor::EventLoop()
{
    while (true)
    {
        HandleEvents();
    }
}
