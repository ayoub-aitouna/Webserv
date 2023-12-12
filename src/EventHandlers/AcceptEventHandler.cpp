#include "AcceptEventHandler.hpp"
#include <openssl/ssl.h>
#include <openssl/err.h>

AcceptEventHandler::AcceptEventHandler(int SocketFd, void *ctx) : EventHandler(SocketFd)
{
    this->ctx = ctx;
}

int AcceptEventHandler::Read()
{
    return -1;
}

int AcceptEventHandler::Write()
{
    return -1;
}

EventHandler *AcceptEventHandler::Accept(void)
{
    struct sockaddr_storage addr;
    socklen_t addrlen;
    int socket;
    void *ssl;

    ssl = NULL;
    addrlen = sizeof(addrlen);
    socket = accept(this->SocketFd, (struct sockaddr *)&addr, &addrlen);
    if (socket < 0)
        throw std::runtime_error("accept() failed");
    if (ctx)
    {
        ssl = OpenSSLLoader::my_SSL_new(ctx);
        if (!ssl)
            throw std::runtime_error("SSL_new() failed");
        OpenSSLLoader::my_SSL_set_fd(ssl, socket);
        if (OpenSSLLoader::my_SSL_accept(ssl) <= 0)
        {
            perror("SSL_accept");
            OpenSSLLoader::my_SSL_shutdown(ssl);
            close(socket);
            OpenSSLLoader::my_SSL_free(ssl);
            throw std::runtime_error("SSL_accept() failed");
        }
    }
    fcntl(socket, F_SETFL, O_NONBLOCK);
    return (new HttpEventHandler(socket, ssl, addr, addrlen));
}