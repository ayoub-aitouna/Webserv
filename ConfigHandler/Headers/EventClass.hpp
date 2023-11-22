
#ifndef EVENTCLASS
#define EVENTCLASS

#include <string>

class EventClass
{
private:
    int worker_connections;

public:
    EventClass(/* args */);
    void Parse(std::string &block);
    ~EventClass();
};
#endif