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
    char buffer[1024];

    int read_bytes;
    read_bytes = recv(this->SocketFd, buffer, sizeof(buffer), 0);
    if (read_bytes <= 0)
        return (0);
    std::cout << "read " << read_bytes << std::endl;
    this->client.data.append(buffer);
    try
    {
        ResourceFile file = this->request.Parse(this->client.data);
        if (file.Fd != -1)
        {
            std::cout << "Created New Respoce Obj" << std::endl;
            this->response = new Response(file);
        }
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
    if (this->response != NULL)
    {
        if (this->response->FlushBuffer(this->SocketFd) == 0)
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
