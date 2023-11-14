#ifndef LSTRING_HPP
#define LSTRING_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

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

#define COLORED(str, color) "\033[1;" << color << "m" << str << "\033[0m"

/**
 * 1-                   activate for all
 * 2-                   activate/deactivated by setting active
 * <any other number>-  deactivated
 */
#ifndef DEBUGE
#define DEBUGE 2
#endif

#if DEBUGE == 1
#define DEBUGOUT(active, x) \
    std::cout << x << std::endl
#elif DEBUGE == 2
#define DEBUGOUT(active, x) \
    if (active)             \
    std::cout << x << std::endl
#else
#define DEBUGOUT(active, x)
#endif

class Lstring
{
private:
    Lstring();
    Lstring &operator=(Lstring &copy);

public:
    static std::vector<std::string> Split(std::string line, std::string delimiter);
    static std::string ExtractFromString(std::string data, std::string start, std::string end);
    static void tolower(std::string &str);
    static void LogAsBinary(std::string &str, bool to_file);
    static std::string RandomStr(int len);
};

#endif
