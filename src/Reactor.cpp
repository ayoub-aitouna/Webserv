#include "Reactor.hpp"

Reactor::Reactor()
{
    if ((this->epoll_fd = epoll_create1(0)) < 0)
        throw std::runtime_error("epoll_create1() failed");
    this->MAX_EVENTS = ConfigHandler::GetEvents().GetWorkerConnections();
    this->events = new struct epoll_event[this->MAX_EVENTS];
}

void Reactor::RegisterSocket(int socketFd, EventHandler *eventHandler)
{
    struct epoll_event event;
    if (eventHandler == NULL)
        return;
    DEBUGOUT(verbose, COLORED("Regester New "
                                  << (dynamic_cast<AcceptEventHandler *>(eventHandler) != NULL ? "Server " : "Client ")
                                  << "Socket " << SSTR(socketFd) << "\n",
                              Blue));
    event.events = EPOLLRDNORM | EPOLLWRNORM;
    event.data.fd = eventHandler->GetSocketFd();
    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, eventHandler->GetSocketFd(), &event) < 0)
        throw std::runtime_error("epoll_ctl() `EPOLL_CTL_ADD` failed" + SSTR(eventHandler->GetSocketFd()));

    this->clients[socketFd] = eventHandler;
}

void Reactor::UnRegisterSocket(int SocketFd)
{
    HttpEventHandler *http_client;
    std::string Type;

    Type = dynamic_cast<AcceptEventHandler *>(this->clients[SocketFd]) != NULL ? "Server " : "Client ";
    DEBUGOUT(verbose, COLORED("UnRegister " << Type << "Socket "
                                            << SSTR(SocketFd) << "\n",
                              Red));
    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, SocketFd, NULL) < 0)
        throw std::runtime_error("epoll_ctl() `EPOLL_CTL_DEL` failed");

    if (this->clients[SocketFd] != NULL)
    {
        delete clients[SocketFd];
        this->clients.erase(SocketFd);
    }
    close(SocketFd);
    http_client = dynamic_cast<HttpEventHandler *>(this->clients[SocketFd]);
    if (http_client && http_client->GetSSL())
    {
        OpenSSLLoader::my_SSL_shutdown(http_client->GetSSL());
        OpenSSLLoader::my_SSL_free(http_client->GetSSL());
    }
}

void Reactor::HandleEvents()
{
    if ((event_count = epoll_wait(this->epoll_fd, this->events, this->MAX_EVENTS, -1)) < 0)
        throw std::runtime_error("epoll_wait() failed");
    Dispatch();
}

void Reactor::Dispatch()
{
    AcceptEventHandler *server;
    HttpEventHandler *client;
    int current_fd;

    client = NULL;
    for (int i = 0; i < this->event_count; i++)
    {
        current_fd = this->events[i].data.fd;
        if (this->events[i].events & EPOLLRDNORM)
        {
            DEBUGOUT(0, "Read Ready current_fd : " << current_fd);

            if ((server = dynamic_cast<AcceptEventHandler *>(this->clients[current_fd])) != NULL)
            {
                try
                {
                    client = dynamic_cast<HttpEventHandler *>(server->Accept());
                    if (client != NULL)
                        return RegisterSocket(client->GetSocketFd(), client);
                }
                catch (std::exception &e)
                {
                    DEBUGOUT(1, "Accept() failed\n"
                                    << e.what() << "\n");
                }
            }
            else if ((client = dynamic_cast<HttpEventHandler *>(this->clients[current_fd])) != NULL)
            {

                client->start = clock();
                if (client->Read() == 0)
                    return UnRegisterSocket(current_fd);
            }
        }
        else if (this->events[i].events & EPOLLWRNORM)
        {
            if ((client = dynamic_cast<HttpEventHandler *>(this->clients[current_fd])) != NULL)
            {
                // /* ****CHECK TIMOUT***** */
                if ((clock() - client->start) > 30 * CLOCKS_PER_SEC)
                    return UnRegisterSocket(current_fd);
                /* ********************* */
                CheckCGIOutput(client);
                if (client->Write() == 0)
                    return UnRegisterSocket(current_fd);
            }
        }
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

Reactor::~Reactor()
{
    delete[] this->events;
    if (close(this->epoll_fd) < 0)
        throw std::runtime_error("close(epoll_fd) failed");
}