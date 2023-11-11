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
#include <netdb.h>
#include <errno.h>
#include "../Lib/Lstring.hpp"
#include <cstring>
#include <stdio.h>

class Server
{
public:
    Server();
    Server(const std::string &node, const std::string &serv);

public:
    void CreatSocket();
    // char *getAddreass();
    void Run();

private:
    std::string node;
    std::string servce;
    int socket_fd;
};

#endif