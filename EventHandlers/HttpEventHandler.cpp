#include "HttpEventHandler.hpp"

HttpEventHandler::HttpEventHandler(int SocketFd, struct sockaddr_storage address, socklen_t address_len) : EventHandler(SocketFd)
{
    this->client.address = address;
    this->client.address_len = address_len;
    this->response = NULL;
    this->start = clock();
}

HttpEventHandler::HttpEventHandler() : EventHandler(-1)
{
    this->client.address_len = sizeof(client.address);
}

int HttpEventHandler::Read()
{
    char buffer[1025];
    int read_bytes;
    bool Parsed;
    read_bytes = read(this->SocketFd, buffer, KB);
    buffer[read_bytes] = 0;
    if (read_bytes <= 0)
        return (0);
    this->start = clock();
    DEBUGOUT(1, "Read " << read_bytes);
    try
    {
        if (this->response != NULL)
            return (read_bytes);
        Parsed = this->request.Parse(CBFTSTR(buffer, read_bytes));
        if (Parsed)
            this->response = new ResponseBuilder(this->request.dataPool);
    }
    catch (const HTTPError &e)
    {
        this->request.dataPool.ResponseStatus = e.statusCode;
        this->response = new ResponseBuilder(this->request.dataPool);
        DEBUGOUT(1, "HTTPError " << COLORED(this->response->StatusCodes[e.statusCode], Red));
    }
    catch (const std::exception &e)
    {
        DEBUGOUT(1, COLORED(e.what(), Red));
        return (0);
    }
    return (read_bytes);
}

void HttpEventHandler::CreateResponse()
{
    this->response = new ResponseBuilder(this->request.dataPool);
}

RequestParser &HttpEventHandler::GetRequestParser()
{
    return this->request;
}

ResponseBuilder *HttpEventHandler::GetResponse()
{
    return (this->response);
}

int HttpEventHandler::Write()
{
    try
    {
        if (this->response != NULL)
        {
            this->start = clock();
            if (this->response->FlushBuffer(this->SocketFd) == 0)
                return (0);
        }
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
    delete this->response;
}
