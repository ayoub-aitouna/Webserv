#include "Lstring.hpp"

std::vector<std::string> Lstring::Split(std::string line, std::string delimiter)
{
    std::vector<std::string> list;
    std::string item;
    size_t pos;
    for (;;)
    {
        if ((pos = line.find(delimiter)) == std::string::npos)
            break;
        item = line.substr(0, pos);
        if (!Lstring::Trim(item, delimiter).empty())
            list.push_back(item);
        line = line.substr(pos + delimiter.size());
    }
    if (!Lstring::Trim(line, delimiter).empty())
        list.push_back(line);
    return (list);
}

size_t GetFirstApearenceOf(std::string line, std::string delimiter)
{
    size_t min_index, index;

    min_index = std::string::npos;
    for (size_t j = 0; j < delimiter.size(); j++)
    {
        if ((index = line.find_first_of(delimiter.at(j))) < min_index)
            min_index = index;
    }
    return (min_index);
}

std::vector<std::string> Lstring::SplitByOneOf(std::string line, std::string delimiter)
{
    std::vector<std::string> list;
    std::stringstream Sline(line);
    std::string Item;
    size_t pos;
    while ((pos = GetFirstApearenceOf(line, delimiter)) != std::string::npos)
    {
        Item = line.substr(0, pos);
        line.erase(0, pos + 1);
        if (!Item.empty())
            list.push_back(Item);
    }
    if (!line.empty())
        list.push_back(line);
    return (list);
}

std::string &Lstring::tolower(std::string &str)
{
    for (std::string::iterator it = str.begin(); it != str.end(); it++)
        *it = std::tolower(*it);
    return (str);
}

std::string Lstring::ExtractFromString(std::string data, std::string start, std::string end, std::string defaultReturn)
{
    size_t start_index, end_index;
    std::string Value;
    if ((start_index = data.find(start)) != std::string::npos)
    {
        Value = data.substr(start_index + start.size());
        if ((end_index = Value.find(end)) != std::string::npos)
            return (Value.substr(0, end_index));
    }
    return (defaultReturn);
}

std::string Lstring::RandomStr(size_t len)
{
    std::string str;
    char buff;
    int fd;
    int i;

    fd = IO::OpenFile("/dev/random", "w+");
    i = 0;
    while (i < 100 && str.length() <= len)
    {
        if (read(fd, &buff, 1) == -1)
            break;
        if (isalnum(buff))
            str += buff;
        i++;
    }
    close(fd);
    return (str);
}

std::string Lstring::encode_binary_to_text(const std::string &input)
{

    std::string output;

    for (std::string::const_iterator it = input.begin(); it != input.end(); it++)
    {
        if (isprint(static_cast<unsigned char>(*it)))
            output.push_back(*it);
        else
        {
            if (*it == '\n')
                output.append("<\\n>\n");
            else if (*it == '\r' && *(it + 1) == '\n')
            {
                output.append("<HttpBeak>\n");
                it++;
            }
            else
                output.push_back('*');
        }
    }

    return output;
}

bool Lstring::Contains(const std::string &input, std::string &checklist, size_t pos)
{
    for (std::string::iterator it = checklist.begin(); it != checklist.end(); it++)
    {
        if (input.find((*it), pos) == std::string::npos)
            return (false);
    }
    return (true);
}

bool Lstring::IsAlNum(const std::string &input, size_t pos, size_t n)
{
    size_t index = 0;
    if (pos >= n)
        return (false);
    for (std::string::const_iterator it = input.begin() + pos; it != input.end() && index < n; it++)
    {
        if (!isalnum(*it))
            return (false);
        index++;
    }
    return (true);
}

std::string &Lstring::Trim(std::string &input, std::string delim)
{
    input.erase(0, input.find_first_not_of(delim));
    input.erase(input.find_last_not_of(delim) + 1);
    return (input);
}

std::string &Lstring::LTrim(std::string &input, std::string delim)
{
    input.erase(0, input.find_first_not_of(delim));
    return (input);
}

std::string &Lstring::RTrim(std::string &input, std::string delim)
{
    input.erase(input.find_last_not_of(delim) + 1);
    return (input);
}

std::string &Lstring::Replace(std::string &input, std::string target, std::string _new)
{
    size_t start, end;
    std::string part1, part2;
    if ((start = input.find(target)) == std::string::npos)
        return (input);
    end = start + target.length();
    part1 = input.substr(0, start);
    part2 = input.substr(end);
    input = (part1 + _new + part2);
    return (input);
}