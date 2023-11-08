#include "Lstring.hpp"

std::vector<std::string> Lstring::Split(std::string line, std::string delimiter)
{
    std::vector<std::string> list;
    while (true)
    {
        size_t pos = line.find(delimiter);
        if (pos == std::string::npos)
        {
            if (line.size() != 0)
                list.push_back(line);
            break;
        }
        list.push_back(line.substr(0, pos));
        line = line.substr(pos + delimiter.size());
    }
    return (list);
}



std::string Lstring::Colored(std::string str, Colors color)
{
    std::ostringstream stream;
    stream << "\033[1;" << color << "m" << str << "\033[0m";
    return stream.str();
}