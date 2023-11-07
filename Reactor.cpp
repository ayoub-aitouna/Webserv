#include "Reactor.hpp"

Reactor::Reactor()
{
}

void Reactor::RegisterSocket(int socketFd, EventHandler *eventHandler)
{
    if (eventHandler == NULL)
        return;
    std::cout << "Regester New Client : " << socketFd << std::endl;
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
    struct pollfd *fds;
    iterator it;
    int i;

    i = 0;
    fds = new struct pollfd[this->clients.size() * sizeof(struct pollfd)];
    for (it = this->clients.begin(); it != this->clients.end(); it++)
    {
        fds[i].fd = it->first;
        fds[i].events = POLLIN | POLLOUT;
        i++;
    }
    if (poll(fds, i, -1) >= 0)
        Dispatch(fds);
    else
        throw std::runtime_error("poll() failled");
}

void Reactor::Dispatch(struct pollfd *fds)
{
    AcceptEventHandler *server;
    HttpEventHandler *client;
    int i;

    i = 0;
    for (iterator it = this->clients.begin(); it != this->clients.end(); it++)
    {
        if (fds[i].events & (POLLIN | POLLERR))
        {
            if ((server = dynamic_cast<AcceptEventHandler *>(it->second)) != NULL)
            {
                client = dynamic_cast<HttpEventHandler *>(server->Accept());
                if (client != NULL)
                    return RegisterSocket(client->GetSocketFd(), client);
            }
            else if ((client = dynamic_cast<HttpEventHandler *>(it->second)) != NULL)
                client->Read();
        }
        if (fds[i].events & (POLLOUT | POLLERR))
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
