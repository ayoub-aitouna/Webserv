#ifndef SERVER_HPP
#define SERVER_HPP
#include <string>
#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <exception>
#include "../Reactor.hpp"

class Server
{
public:
    Server();
    Server(const std::string &node, const std::string &serv);

public:
    void CreatSocket();
    void Run();

private:
    std::string node;
    std::string servce;
    int socket_fd;
};

struct addrinfo
{
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ai_addrlen;
    struct sockaddr *ai_addr;
    char *ai_canonname;       /* canonical name */
    struct addrinfo *ai_next; /* this struct can form a linked list */
};

int getaddrinfo(const char *node,
                const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res);

void freeaddrinfo(struct addrinfo *res);

#endif