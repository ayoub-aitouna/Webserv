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
            if (*it == '\r' && *(it + 1) == '\n')
            {
                output.append("<brk>\n");
                it++;
            }
            else
                output.push_back('*');
        }
    }

    return output;
}
