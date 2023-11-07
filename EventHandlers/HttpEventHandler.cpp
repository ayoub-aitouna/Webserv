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
    std::cout << "recving Data " << std::endl;
    int readed_bytes = recv(this->SocketFd, buffer, sizeof(buffer), 0);
    this->request.append(buffer);
    return (readed_bytes);
}

int HttpEventHandler::Write()
{
    std::string responce = "HTTP/1.1 404 Not Found \r\n"
                           "Content-Length: 9 \r\n"
                           "Content-Type: text/html \r\n"
                           "Connection: Closed \r\n\r\n"
                           "Not Found";
    std::cout << "sending Data " << std::endl;
    send(this->SocketFd, responce.c_str(), responce.size(), 0);
    return (0);
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
