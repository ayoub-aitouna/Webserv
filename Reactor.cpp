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
    iterator it;
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
    struct pollfd *fds = new struct pollfd[this->clients.size() * sizeof(struct pollfd)];
    iterator it;
    int i = 0;
    for (it = this->clients.begin(); it != this->clients.end(); it++)
    {
        fds[i].fd = it->first;
        fds[i].events = POLLIN | POLLOUT;
        i++;
    }
    if (poll(fds, i, 3000) >= 0)
        Dispatch(fds);
    else
        throw std::runtime_error("poll() failled");
}

void Reactor::Dispatch(struct pollfd *fds)
{
    int i;
    AcceptEventHandler *server;
    HttpEventHandler *client;

    i = 0;
    for (iterator it = this->clients.begin(); it != this->clients.end(); it++)
    {
        if (fds[i].events & (POLLIN))
        {

            if ((server = dynamic_cast<AcceptEventHandler *>(it->second)) != NULL)
            {
                client = dynamic_cast<HttpEventHandler *>(server->Accept());
                RegisterSocket(client->GetSocketFd(), client);
            }
            else if ((client = dynamic_cast<HttpEventHandler *>(it->second)) != NULL)
                client->Read();
        }
        if (fds[i].events & (POLLERR | POLLWRNORM))
        {
            if ((client = dynamic_cast<HttpEventHandler *>(it->second)) != NULL)
            {
                if (client->Write() == 0)
                    UnRegisterSocket(it->first);
            }
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
