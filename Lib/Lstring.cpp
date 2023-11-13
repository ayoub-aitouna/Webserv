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

void Lstring::tolower(std::string &str)
{
    for (std::string::iterator it = str.begin(); it != str.end(); it++)
        *it = std::tolower(*it);
}
std::string Lstring::ExtractFromString(std::string data, std::string start, std::string end)
{
    size_t start_index, end_index;
    std::string Value = "";
    if ((start_index = data.find(start)) != std::string::npos)
    {
        Value = data.substr(start_index + start.size());
        if ((end_index = Value.find(end)) != std::string::npos)
            return (Value.substr(0, end_index));
    }
    return ("");
}