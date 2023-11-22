#ifndef CONFIGHANDLER_HPP
#define CONFIGHANDLER_HPP
#include <string>
#include "EventClass.hpp"
#include "HttpClass.hpp"
#include "Preprocessor.hpp"

class ConfigHandler
{

public:
    ConfigHandler(std::string FileName);
    void Parse();
    ~ConfigHandler();

private:
    std::string RawData;
    EventClass *events;
    std::vector<HttpClass> http;
};
#endif