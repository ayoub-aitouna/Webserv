#ifndef EVENTHANDLER_HPP
#define EVENTHANDLER_HPP
#include <vector>
#include <map>
class EventHandler
{
public:
    virtual int Read() = 0;
    virtual int Write() = 0;
};

#endif