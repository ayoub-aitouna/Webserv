#ifndef ACCEPTEVENTHANDLER_HPP
#define ACCEPTEVENTHANDLER_HPP
#include <vector>
#include <map>
#include "EventHandler.hpp"
class AcceptEventHandler : public EventHandler
{
public:
    AcceptEventHandler();

public:
    void Accept();
};

#endif