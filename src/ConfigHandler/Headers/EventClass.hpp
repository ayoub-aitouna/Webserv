
#ifndef EVENTCLASS
#define EVENTCLASS

#include <string>
#include "ServerClass.hpp"
#include "TypesClass.hpp"
#include "Preprocessor.hpp"

class EventClass
{
private:
    std::string RawData;
    int worker_connections;

public:
    EventClass();
    void SetRawData(std::string &RawData);
    void Parse();
    void DisplayValues(bool Show);
    size_t GetWorkerConnections();
    ~EventClass();
};
#endif