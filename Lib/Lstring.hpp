#ifndef LSTRING_HPP
#define LSTRING_HPP

#include <string>
#include <vector>

class Lstring
{
private:
    Lstring();
    Lstring &operator=(Lstring &copy);

public:
    static std::vector<std::string> Split(std::string line, std::string delimiter);
};

#endif
