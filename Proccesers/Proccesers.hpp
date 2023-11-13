#ifndef PROCCESERS
#define PROCCESERS

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "../Lib/Lstring.hpp"
#include "../Lib/MimeTypes.hpp"
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <fstream>
#include <iostream>

#define KB 1024
#define SSTR(x) static_cast<std::ostringstream &>(std::ostringstream() << std::dec << x).str()

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
    int ResponseStatus;
    int Method;
} DataPool;

typedef std::map<std::string, std::string>::iterator HeadersIterator;

#endif