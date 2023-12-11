#include "Server.hpp"

Server::Server()
{
}

const char *get_cstr(std::string str)
{
    return (str == "" ? NULL : str.c_str());
}

void Server::CreatSocket(ServerClass conf)
{
    struct addrinfo hint;
    struct addrinfo *addr;
    int resure_flag;
    int socket_fd;

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;

    if (getaddrinfo(get_cstr(conf.GetHostName()), get_cstr(conf.GetPort()), &hint, &addr) < 0)
        throw std::runtime_error("getaddrinfo() failed");

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
    VirtualServers.push_back(VirtualServer(socket_fd, conf.IsSSlOn(),
                                           conf.GetSSlCertificate(), conf.GetSSlcertificatekey()));
}

void Server::Run()
{
    Reactor reactor;
    DEBUGOUT(1, COLORED("WebServer Starting...", Green));
    for (std::vector<VirtualServer>::iterator it = VirtualServers.begin(); it != VirtualServers.end(); it++)
    {
        if (listen(it->GetSocketFd(), 100) < 0)
            throw std::runtime_error("listen() failed");
        reactor.RegisterSocket(it->GetSocketFd(), new AcceptEventHandler(it->GetSocketFd(), it->getSLL_CTX()));
    }
    DEBUGOUT(1, COLORED("Ready....", Green));
    reactor.EventLoop();
}