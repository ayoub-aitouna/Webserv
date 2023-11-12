#include "HttpEventHandler.hpp"

HttpEventHandler::HttpEventHandler(int SocketFd, struct sockaddr_storage address, socklen_t address_len) : EventHandler(SocketFd)
{
    this->client.address = address;
    this->client.address_len = address_len;
    this->response = NULL;
}

HttpEventHandler::HttpEventHandler() : EventHandler(-1)
{
    this->client.address_len = sizeof(client.address);
}

int HttpEventHandler::Read()
{
    char buffer[1025];
    Medium *medium;

    int read_bytes;
    read_bytes = recv(this->SocketFd, buffer, KB, 0);
    buffer[read_bytes] = 0;
    if (read_bytes <= 0)
        return (0);
    DEBUGOUT(0, "read " << read_bytes);
    try
    {
        medium = this->request.Parse(CBFTSTR(buffer, read_bytes));
        if (medium != NULL)
            this->response = new ResponseBuilder(*medium);
    }
    catch (const RequestParser::HTTPError &e)
    {

        Medium *medium = new Medium(e.statusCode);
        this->response = new ResponseBuilder(*medium);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return (0);
    }
    return (read_bytes);
}

int HttpEventHandler::Write()
{
    try
    {
        if (this->response != NULL && this->response->FlushBuffer(this->SocketFd) == 0)
            return (0);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return (0);
    }

    return (-1);
}

EventHandler *HttpEventHandler::Accept(void)
{
    return (NULL);
}

const int &HttpEventHandler::GetSocketFd() const
{
    return this->SocketFd;
}

HttpEventHandler::~HttpEventHandler()
{
}
