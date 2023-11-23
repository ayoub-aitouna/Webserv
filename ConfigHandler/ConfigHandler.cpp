
#include "Headers/ConfigHandler.hpp"

std::string ConfigHandler::RawData;
EventClass ConfigHandler::events;
HttpClass ConfigHandler::http;

ConfigHandler::ConfigHandler()
{
}

void ConfigHandler::SetFile(std::string FileName)
{
    try
    {
        std::ifstream inputFile(FileName.c_str());
        if (!inputFile)
            throw std::runtime_error("Invalide File");
        Preprocessor processor(inputFile);
        ConfigHandler::RawData = processor.Processor();
        Parse();
    }
    catch (const std::exception &e)
    {
        std::cerr << COLORED(e.what(), Red) << std::endl;
    }
}

void ConfigHandler::Parse()
{
    std::vector<std::string> Blocks;

    Blocks = ExtractBlock(ConfigHandler::RawData, "events");
    if (Blocks.size() != 1)
        throw std::runtime_error("It Should be one `events` Block");
    DEBUGOUT(1, Blocks.at(0));
    ConfigHandler::events.SetRawData(Blocks.at(0));
    ConfigHandler::events.Parse();
    Blocks = ExtractBlock(ConfigHandler::RawData, "http");
    if (Blocks.size() != 1)
        throw std::runtime_error("It Should be at one `http` Block");
    for (size_t i = 0; i < Blocks.size(); i++)
        http.SetRawData(Blocks.at(i));
}

HttpClass &ConfigHandler::GetHttp()
{
    return ConfigHandler::http;
}

EventClass &ConfigHandler::GetEvents()
{
    return ConfigHandler::events;
}

ConfigHandler::~ConfigHandler()
{
}
