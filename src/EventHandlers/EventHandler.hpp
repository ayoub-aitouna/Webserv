#ifndef EVENTHANDLER_HPP
#define EVENTHANDLER_HPP
#include <vector>
#include <map>
#include <iostream>
// #include <openssl/ssl.h>


class EventHandler
{
public:
    EventHandler(int SocketFd);
    virtual int Read() = 0;
    virtual int Write() = 0;
    virtual EventHandler *Accept() = 0;
    const int &GetSocketFd() const;
    virtual ~EventHandler();

private:
    EventHandler();

protected:
    int SocketFd;
    void *ssl;
};

#endif