#include "IO.hpp"
int IO::OpenFile(const char *path, const char *mode)
{
    FILE *file;
    int fd;

    file = std::fopen(path, mode);
    if (!file)
        throw std::runtime_error("Error while Opening a file");
    fd = file->_fileno;
    if (fd < 0)
        throw std::runtime_error("Error while Opening a file");
    if (fcntl(file->_fileno, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Error while setting non-block to file");
    return (fd);
}