#ifndef EVENTHANDLER_HPP
#define EVENTHANDLER_HPP
#include <vector>
#include <map>
#include <iostream>

class EventHandler
{
public:
    EventHandler(int SocketFd);
    virtual int Read() = 0;
    virtual int Write() = 0;
    virtual EventHandler *Accept() = 0;
    virtual ~EventHandler();
    const int& GetSocketFd() const;

private:
    EventHandler();

protected:
    int SocketFd;
};

#endif