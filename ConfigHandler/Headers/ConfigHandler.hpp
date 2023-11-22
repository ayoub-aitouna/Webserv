#ifndef CONFIGHANDLER
#define CONFIGHANDLER

#include <string>
#include "EventClass.hpp"

class ConfigHandler
{

public:
    ConfigHandler(/* args */);
    void Parse(std::string &block);
    ~ConfigHandler();

private:
    EventClass events;
};
#endif