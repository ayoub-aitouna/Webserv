#include "Server.hpp"

Server::Server() : node(""), servce("")
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

    const char *node = this->node == "" ? NULL : this->node.c_str();
    const char *servce = this->servce == "" ? NULL : this->servce.c_str();

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;

    DEBUGOUT(1, COLORED("Creating Socket", White));
    getaddrinfo(node, servce, &hint, &addr);
    this->socket_fd = socket(addr->ai_family, addr->ai_socktype, 0);
    if (this->socket_fd < 0)
    {
        perror("");
        throw std::runtime_error("socket() failed");
    }

    resure_flag = 1;
    if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &resure_flag, sizeof(resure_flag)) < 0)
        throw std::runtime_error("setsockopt() failed");
    DEBUGOUT(1, COLORED("Binding Socket", Yellow));
    if (bind(this->socket_fd, addr->ai_addr, addr->ai_addrlen) < 0)
    {
        perror("bind() ");
        throw std::runtime_error("bind() failed");
    }
    freeaddrinfo(addr);
}

void Server::Run()
{
    Reactor reactor;

    if (listen(this->socket_fd, 10) < 0)
        throw std::runtime_error("listen() failed");
    DEBUGOUT(1, COLORED("Server in Listen Mode", Green));
    reactor.RegisterSocket(this->socket_fd, new AcceptEventHandler(this->socket_fd));
    reactor.EventLoop();
}