#ifndef PREPROCESSOR
#define PREPROCESSOR

#include <string>
#include <iostream>
#include <sstream>
#include <stack>
#include "../../Lib/Lstring.hpp"
#include "../../Lib/IO.hpp"

class Preprocessor
{
private:
   std::ifstream &inputFile;

public:
    Preprocessor(std::ifstream &inputFile);
    std::string Processor();
    ~Preprocessor();
};
std::vector<std::string> ExtractBlock(std::string &data, std::string BlockName);
#endif