#ifndef PROCCESERS
#define PROCCESERS

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "../Lib/Lstring.hpp"
#include "../Lib/IO.hpp"
#include "../Lib/MimeTypes.hpp"
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define KB 1024
#define SSTR(x) static_cast<std::ostringstream &>(std::ostringstream() << std::dec << x).str()

enum
{
    NOBODY = -1,
    ERROR = -2,
};

enum
{
    Lenght,
    Chunked,
    Connection,
    NON
};

enum
{
    GET,
    POST,
    DELETE,
    OTHER
};

enum
{
    WB_FILE,
    WB_DIRECTORY,
    WB_NEITHER
};

enum
{
    Size,
    Data,
    End,
    Trailer,
    Error
};

typedef struct S_File
{
    int Fd;
    std::string ResourceFileType;
} WBSRVFILE;

typedef struct S_DataPool
{
    std::map<std::string, std::string> Headers;
    WBSRVFILE File;
    std::string Url;
    std::string Query;
    std::string body;
    std::string Location;
    int ResponseStatus;
    int ResourceType;
    int Method;
} DataPool;

typedef std::map<std::string, std::string>::iterator HeadersIterator;

#endif