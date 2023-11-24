#include "Lstring.hpp"

std::vector<std::string> Lstring::Split(std::string line, std::string delimiter)
{
    std::vector<std::string> list;
    std::string item;
    while (true)
    {
        size_t pos = line.find(delimiter);
        if (pos == std::string::npos)
        {
            if (line.size() != 0)
            {
                Lstring::Trim(line, delimiter);
                if (!line.empty())
                    list.push_back(line);
            }
            break;
        }
        item = line.substr(0, pos);
        Lstring::Trim(item, delimiter);
        if (!item.empty())
            list.push_back(item);
        line = line.substr(pos + delimiter.size());
    }
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

void Lstring::LogAsBinary(std::string &str, bool to_file)
{

    int fd = to_file ? IO::OpenFile("info_log.log", "a+") : 1;
    int i = 0;
    for (std::string::iterator it = str.begin(); it < str.end(); it++)
    {
        dprintf(fd, "%.8x ", static_cast<int>(*it));
        if (i == 8)
        {
            dprintf(fd, "\n");
            i = 0;
        }
        else
            i++;
    }
    close(fd);
    dprintf(fd, "\n");
}

long Lstring::Stol(const std::string &str,
                   std::size_t *pos, int base)
{
    char *End;
    size_t index = 0;
    long num;

    num = strtol(str.c_str(), &End, base);

    for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
    {
        if (&(*it) == End)
            break;
        index++;
    }
    if (pos != NULL)
        *pos = (index >= str.length()) ? std::string::npos : index;
    return (num);
}

std::string Lstring::base64_encode(const std::string &input)
{

    std::string output;

    for (std::string::const_iterator it = input.begin(); it != input.end(); it++)
    {
        // Check if the character is printable
        if (isprint(static_cast<unsigned char>(*it)))
        {
            output.push_back(*it);
        }
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

std::string Lstring::SkipLine(std::string &input, std::string endl)
{
    size_t index;

    if ((index = input.find(endl)) != std::string::npos)
        input = input.substr(index + endl.size());
    return (input);
}

std::string Lstring::GetLine(std::string input, std::string endl, size_t LineIndex)
{
    size_t index;
    size_t curLineIndex = 0;
    std::string Line;
    while (curLineIndex <= LineIndex)
    {
        if ((index = input.find(endl)) != std::string::npos)
        {
            if (input.substr(0, index + endl.size()) == endl)
                return (endl);
            Line = input.substr(0, index);
            input = input.substr(index + endl.size());
        }
        else
            return "";
        curLineIndex++;
    }
    return (Line);
}

std::string Lstring::SkipLineIfContains(std::string &input, std::string content, std::string endl)
{
    size_t index;
    std::string Line;

    if ((index = input.find(endl)) != std::string::npos)
    {
        Line = input.substr(0, index);
        if ((index = Line.find(content)) != std::string::npos)
            SkipLine(input, endl);
    }
    return (Line);
}

void Lstring::Trim(std::string &input, std::string delim)
{
    input.erase(0, input.find_first_not_of(delim));
    input.erase(input.find_last_not_of(delim) + 1);
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