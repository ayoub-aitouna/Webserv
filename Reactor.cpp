#include "Reactor.hpp"

Reactor::Reactor()
{
}

void Reactor::RegisterSocket(int socketFd, EventHandler *eventHandler)
{

    if (eventHandler == NULL)
        return;
    DEBUGOUT(0, COLORED(std::string("Regester New ")
                         << (dynamic_cast<AcceptEventHandler *>(eventHandler) != NULL ? "Server " : "Client ")
                         << "Socket " << SSTR(socketFd),
                     Blue));
    this->clients.push_back(std::make_pair(socketFd, eventHandler));
}

void Reactor::UnRegisterSocket(int SocketFd)
{
    iterator it;

    for (it = this->clients.begin(); it != this->clients.end(); it++)
    {
        if (it->first == SocketFd)
        {
            DEBUGOUT(0, COLORED(std::string("UnRegister ")
                                 << (dynamic_cast<AcceptEventHandler *>(it->second) != NULL ? "Server " : "Client ")
                                 << "Socket "
                                 << SSTR(SocketFd),
                             Red));
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
        fds[i].events = POLLWRNORM | POLLRDNORM;
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
        if (fds[i].revents & POLLRDNORM)
        {
            if ((server = dynamic_cast<AcceptEventHandler *>(it->second)) != NULL)
            {
                client = dynamic_cast<HttpEventHandler *>(server->Accept());
                if (client != NULL)
                    return RegisterSocket(client->GetSocketFd(), client);
            }
            else if ((client = dynamic_cast<HttpEventHandler *>(it->second)) != NULL)
            {
                if (client->Read() == 0)
                    return UnRegisterSocket(it->first);
            }
        }
        if (fds[i].revents & POLLWRNORM)
        {
            if ((client = dynamic_cast<HttpEventHandler *>(it->second)) != NULL)
            {
                if (client->Write() == 0)
                    return UnRegisterSocket(it->first);
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
