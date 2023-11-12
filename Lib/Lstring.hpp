#ifndef LSTRING_HPP
#define LSTRING_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

typedef enum Colors
{
    Black = 30,
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Magenta = 35,
    Cyan = 36,
    White = 37,
    Reset = 0,
} t_colors;

class Lstring
{
private:
    Lstring();
    Lstring &operator=(Lstring &copy);

public:
    static std::string Colored(std::string str, t_colors color);
    static std::vector<std::string> Split(std::string line, std::string delimiter);
    static void tolower(std::string &str);
};

#endif
