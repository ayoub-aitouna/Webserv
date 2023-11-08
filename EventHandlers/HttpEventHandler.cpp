#include "HttpEventHandler.hpp"

#include <assert.h>

HttpEventHandler::HttpEventHandler(int SocketFd, struct sockaddr_storage address, socklen_t address_len) : EventHandler(SocketFd), request(""), address(address), address_len(address_len)
{
}

HttpEventHandler::HttpEventHandler() : EventHandler(-1), address_len(sizeof(address))
{
}

int HttpEventHandler::Read()
{
    char buffer[1024];

    int read_bytes;
    std::cout << "Readinig " << std::endl;
    read_bytes = recv(this->SocketFd, buffer, sizeof(buffer), 0);
    if (read_bytes <= 0)
        return (0);
    std::cout << "read " << read_bytes << std::endl;
    this->request.append(buffer);
    this->Serializer.Deserialize(this->request);
    return (read_bytes);
}

int HttpEventHandler::Write()
{
    std::string responce = "HTTP/1.1 404 Not Found \r\n"
                           "Content-Length: 9 \r\n"
                           "Content-Type: text/html \r\n"
                           "Connection: Closed \r\n\r\n"
                           "Not Found";

    if (this->Serializer.DoneDeserializing)
    {
        std::cout << "Wrinting" << std::endl;
        send(this->SocketFd, responce.c_str(), responce.size(), 0);
        std::cout << "DoneDeserializing" << std::endl;
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
