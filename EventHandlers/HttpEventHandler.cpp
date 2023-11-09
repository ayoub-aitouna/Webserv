#include "HttpEventHandler.hpp"

#include <assert.h>

HttpEventHandler::HttpEventHandler(int SocketFd, struct sockaddr_storage address, socklen_t address_len) : EventHandler(SocketFd)
{
    this->address = address;
    this->address_len = address_len;
}

HttpEventHandler::HttpEventHandler() : EventHandler(-1), address_len(sizeof(address))
{
}

int HttpEventHandler::Read()
{
    char buffer[1024];

    int read_bytes;
    read_bytes = recv(this->SocketFd, buffer, sizeof(buffer), 0);
    if (read_bytes <= 0)
        return (0);
    std::cout << "read " << read_bytes << std::endl;
    this->request.append(buffer);
    try
    {
        this->Serializer.Deserialize(this->request);
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
    if (this->Serializer.GetResponse() != NULL)
    {
        if (this->Serializer.GetResponse()->FlushBuffer(this->SocketFd) == 0)
            return (0);
        this->Serializer.GetResponse()->FillBuffer();
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

void HttpEventHandler::SetSocketFd(const int &SocketFd)
{
    this->SocketFd = SocketFd;
}

struct sockaddr_storage &HttpEventHandler::GetClientAddr()
{
    return this->address;
}

socklen_t &HttpEventHandler::GetClientAddrLen()
{
    return this->address_len;
}

HttpEventHandler::~HttpEventHandler()
{
}
