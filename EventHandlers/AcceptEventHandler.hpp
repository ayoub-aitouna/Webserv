#ifndef ACCEPTEVENTHANDLER_HPP
#define ACCEPTEVENTHANDLER_HPP
#include <vector>
#include <map>
#include "EventHandler.hpp"
#include "../Reactor.hpp"

class AcceptEventHandler : public EventHandler
{

public:
    AcceptEventHandler(int SocketFd);

public:
    int Read();
    int Write();
    EventHandler *Accept();
};

#endif