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
    std::map<std::string, std::string> Content_Types;
    std::map<std::string, std::string> Reverse_Content_Types;
    int ResponseStatus;
    int ResourceType;
    int Method;
} DataPool;

typedef std::map<std::string, std::string>::iterator HeadersIterator;

std::string GetHeaderAttr(DataPool &dataPool, std::string name);

#endif