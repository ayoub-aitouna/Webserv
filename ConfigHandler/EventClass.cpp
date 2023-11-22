#include "Headers/EventClass.hpp"

EventClass::EventClass(std::string &RawData) : RawData(RawData)
{
}

void EventClass::SetRawData(std::string &RawData)
{
     this->RawData = RawData;
}

void EventClass::Parse()
{
}

EventClass::~EventClass()
{
}
