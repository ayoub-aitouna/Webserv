#include "Server.hpp"

Server::Server() : node(NULL), servce(NULL)
{
}

Server::Server(const std::string &node, const std::string &serv) : node(node), servce(serv)
{
}

void Server::CreatSocket()
{
    struct addrinfo hint;
    struct addrinfo *addr;
    int resure_flag;

    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;

    getaddrinfo(node.c_str(), servce.c_str(), &hint, &addr);
    this->socket_fd = socket(addr->ai_family, addr->ai_socktype, 0);
    if (this->socket_fd < 0)
        throw std::runtime_error("socket() failed");
    resure_flag = 1;
    if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &resure_flag, sizeof(resure_flag)) < 0)
        throw std::runtime_error("setsockopt() failed");

    if (bind(this->socket_fd, addr->ai_addr, addr->ai_addrlen) < 0)
        throw std::runtime_error("bind() failed");
    freeaddrinfo(addr);
}

void Server::Run()
{
    if (listen(this->socket_fd, 10) < 0)
        throw std::runtime_error("listen() failed");
}