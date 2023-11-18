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

public:
    void CreatSocket(const std::string &node, const std::string &serv);
    void Run();

private:
    std::vector<int> VirtualServersFd;
};

#endif