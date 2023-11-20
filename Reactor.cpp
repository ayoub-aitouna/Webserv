#include "Reactor.hpp"

Reactor::Reactor()
{
    this->times = 0;
}

void Reactor::RegisterSocket(int socketFd, EventHandler *eventHandler)
{

    if (eventHandler == NULL)
        return;
    DEBUGOUT(1, COLORED(std::string("Regester New ")
                            << (dynamic_cast<AcceptEventHandler *>(eventHandler) != NULL ? "Server " : "Client ")
                            << "Socket " << SSTR(socketFd) << "\n",
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
            DEBUGOUT(1, COLORED(std::string("UnRegister ")
                                    << (dynamic_cast<AcceptEventHandler *>(it->second) != NULL ? "Server " : "Client ")
                                    << "Socket "
                                    << SSTR(SocketFd) << "\n",
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
    {
        times++;
        DEBUGOUT(0, COLORED("\e[A\r\e[0K"
                                << "Dispatch " << times,
                            Yellow));
        Dispatch(fds);
    }
    else
        throw std::runtime_error("poll() failled");
    delete fds;
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
            DEBUGOUT(0, COLORED("\e[A\r\e[0K"
                                    << "Readable " << times,
                                Yellow));
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
        else if (fds[i].revents & POLLWRNORM)
        {

            if ((client = dynamic_cast<HttpEventHandler *>(it->second)) != NULL)
            {
                /* ****CHECK TIMOUT***** */
                // clock_t diff = clock() - client->start;
                // if (diff > 10 * 3 * 1000)
                //     return UnRegisterSocket(it->first);
                /* ********************* */
     

                if (client->GetResponse() == NULL)
                    CheckCGIOutput(client);

                if (client->Write() == 0)
                    return UnRegisterSocket(it->first);
            }
        }
        i++;
    }
}

void CheckCGIOutput(HttpEventHandler *client)
{
    Request *RequestHandler;

    if ((RequestHandler = client->GetRequestParser().GetRequestHandler()))
    {
        try
        {
            if (RequestHandler->ParseCGIOutput())
                client->CreateResponse();
        }
        catch (const HTTPError &e)
        {
            client->GetRequestParser().dataPool.ResponseStatus = e.statusCode;
            client->CreateResponse();
        }
    }
}

void Reactor::EventLoop()
{
    while (true)
    {
        HandleEvents();
    }
}
