#include "Server.hpp"

Server::Server()
{
}

void Server::CreatSocket(const std::string &node, const std::string &serv)
{
    struct addrinfo hint;
    struct addrinfo *addr;
    int resure_flag;
    int socket_fd;

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;

    const char *str_node = node == "" ? NULL : node.c_str();
    const char *str_servce = serv == "" ? NULL : serv.c_str();

    getaddrinfo(str_node, str_servce, &hint, &addr);

    
    socket_fd = socket(addr->ai_family, addr->ai_socktype, 0);
    if (socket_fd < 0)
    {
        perror("socket: ");
        throw std::runtime_error("socket() failed");
    }

    resure_flag = 1;
    
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &resure_flag, sizeof(resure_flag)) < 0)
        throw std::runtime_error("setsockopt() failed");

    if (bind(socket_fd, addr->ai_addr, addr->ai_addrlen) < 0)
    {
        perror("bind() ");
        throw std::runtime_error("bind() failed");
    }
    freeaddrinfo(addr);
    VirtualServersFd.push_back(socket_fd);
}

void Server::Run()
{
    Reactor reactor;
    DEBUGOUT(1, COLORED("WebServer Starting...", Green));
    for (std::vector<int>::iterator it = VirtualServersFd.begin(); it != VirtualServersFd.end(); it++)
    {
        if (listen(*it, 100) < 0)
            throw std::runtime_error("listen() failed");
        reactor.RegisterSocket(*it, new AcceptEventHandler(*it));
    }
    DEBUGOUT(1, COLORED("Ready....", Green));
    reactor.EventLoop();
}