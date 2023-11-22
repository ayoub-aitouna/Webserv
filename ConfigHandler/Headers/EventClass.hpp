
#ifndef EVENTCLASS
#define EVENTCLASS

#include <string>

class EventClass
{
private:
    std::string RawData;
    int worker_connections;

public:
    EventClass(std::string &RawData);
    void SetRawData(std::string &RawData);
    void Parse();
    ~EventClass();
};
#endif