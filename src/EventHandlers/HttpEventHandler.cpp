#include "HttpEventHandler.hpp"

ssize_t WB_read(int __fd, void *__ssl, void *__buf, size_t __nbytes)
{
    if (__ssl)
        return OpenSSLLoader::my_SSL_read(__ssl, __buf, __nbytes);
    return read(__fd, __buf, __nbytes);
}

ssize_t WB_write(int __fd, void *__ssl, char *__buf, size_t __nbytes)
{
    if (__ssl)
        return OpenSSLLoader::my_SSL_read(__ssl, __buf, __nbytes);
    return read(__fd, __buf, __nbytes);
}

HttpEventHandler::HttpEventHandler(int SocketFd, void *ssl, struct sockaddr_storage address, socklen_t address_len) : EventHandler(SocketFd)
{
    this->client.address = address;
    this->client.address_len = address_len;
    this->ssl = ssl;
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

    read_bytes = WB_read(this->SocketFd, this->ssl, buffer, KB);
    if (read_bytes <= 0)
        return (0);
    this->start = clock();
    try
    {
        if (this->response != NULL)
            return (read_bytes);
        if (this->request.Parse(CBFTSTR(buffer, read_bytes)))
            this->response = new ResponseBuilder(this->request.GetDataPool());
    }
    catch (const HTTPError &e)
    {
        this->request.GetDataPool().ResponseStatus = e.statusCode;
        this->response = new ResponseBuilder(this->request.GetDataPool());
        DEBUGOUT(0, "HTTPError " << COLORED(this->response->GetStatusCodes()[e.statusCode], Red));
    }
    catch (const std::exception &e)
    {
        this->request.GetDataPool().ResponseStatus = 500;
        this->response = new ResponseBuilder(this->request.GetDataPool());
        DEBUGOUT(1, "HTTPError " << COLORED(this->response->GetStatusCodes()[500] << "\nReason:\n\t" << e.what(), Red));
    }
    return (read_bytes);
}

void HttpEventHandler::CreateResponse(HeadersType &ResponseHeaders)
{
    this->response = new ResponseBuilder(this->request.GetDataPool(), ResponseHeaders);
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
            if (this->response->FlushBuffer(this->SocketFd, this->ssl) == 0)
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

Request *HttpEventHandler::GetRequestHandler()
{
    return this->request.GetRequestHandler();
}
void *HttpEventHandler::GetSSL()
{
    return (this->ssl);
}

HttpEventHandler::~HttpEventHandler()
{
    delete this->response;
}
