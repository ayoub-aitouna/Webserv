#include "Reactor.hpp"

Reactor::Reactor()
{
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

    if (poll(fds, i, -1) < 0)
        throw std::runtime_error("poll() failled");
    Dispatch(fds);
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

            if ((server = dynamic_cast<AcceptEventHandler *>(it->second)) != NULL)
            {
                client = dynamic_cast<HttpEventHandler *>(server->Accept());
                if (client != NULL)
                    return RegisterSocket(client->GetSocketFd(), client);
            }
            else if ((client = dynamic_cast<HttpEventHandler *>(it->second)) != NULL)
            {

                client->start = clock();
                if (client->Read() == 0)
                    return UnRegisterSocket(it->first);
            }
        }
        else if (fds[i].revents & POLLWRNORM)
        {
            if ((client = dynamic_cast<HttpEventHandler *>(it->second)) != NULL)
            {
                // /* ****CHECK TIMOUT***** */
                if ((clock() - client->start) > 30 * CLOCKS_PER_SEC)
                    return UnRegisterSocket(it->first);
                /* ********************* */

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
    HeadersType ResponseHeaders;

    if ((RequestHandler = client->GetRequestHandler()))
    {
        if (client->GetResponse() != NULL ||
            !RequestHandler->GetRunningProcessId())
            return;
        try
        {
            if (RequestHandler->GetCGIController()->ParseCGIOutput(ResponseHeaders))
                client->CreateResponse(ResponseHeaders);
        }
        catch (const HTTPError &e)
        {
            client->GetRequestParser().GetDataPool().ResponseStatus = e.statusCode;
            client->CreateResponse(ResponseHeaders);
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
