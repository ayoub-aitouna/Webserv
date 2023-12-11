#ifndef PREPROCESSOR
#define PREPROCESSOR

#include <string>
#include <iostream>
#include <sstream>
#include <stack>
#include <fstream>
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

template <typename T>
bool Containes(std::vector<T> Vec, T item)
{
    for (typename std::vector<T>::iterator i = Vec.begin(); i != Vec.end(); i++)
    {
        if (*i == item)
            return (true);
    }
    return (false);
}

#endif