
#include "Headers/ConfigHandler.hpp"

ConfigHandler::ConfigHandler(std::string FileName)
{
    std::ifstream inputFile(FileName.c_str());
    if (!inputFile)
        throw std::runtime_error("Invalide File");
    Preprocessor processor(inputFile);
    this->RawData = processor.Processor();
    Parse();
}

void ConfigHandler::Parse()
{
    std::vector<std::string> Blocks;

    Blocks = ExtractBlock(this->RawData, "events");
    if (Blocks.size() != 1)
        throw std::runtime_error("more or less then 1 `events` Block");
    this->events = new EventClass(Blocks.at(0));
    Blocks = ExtractBlock(this->RawData, "http");
    if (!Blocks.size())
        throw std::runtime_error("It Should be at least one `http` Block");
    for (size_t i = 0; i < Blocks.size(); i++)
        http.push_back(HttpClass(Blocks.at(i)));
}

ConfigHandler::~ConfigHandler()
{
    
}
