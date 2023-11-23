#ifndef CONFIGHANDLER_HPP
#define CONFIGHANDLER_HPP
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "EventClass.hpp"
#include "HttpClass.hpp"

class ConfigHandler
{

public:
    ConfigHandler();
    static void SetFile(std::string FileName);
    static void Parse();
    static HttpClass &GetHttp();
    static EventClass &GetEvents();
    ~ConfigHandler();

private:
    static std::string RawData;
    static EventClass events;
    static HttpClass http;
};
#endif