#ifndef IO_HPP
#define IO_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

#include <cstdio>
#include <cstdlib>

// #include "../Core/Include/Core.hpp"

class IO
{
private:
    IO();
    IO &operator=(IO &copy);

public:
    static int OpenFile(const char *path, const char *mode);
};

#endif
