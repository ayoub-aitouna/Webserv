#ifndef EVENTHANDLER_HPP
#define EVENTHANDLER_HPP
#include <vector>
#include <map>
class EventHandler
{
public:
    EventHandler();

public:
    virtual void Read() = 0;
    virtual void Write() = 0;
};

#endif