#ifndef Core
#define Core

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "../../Lib/Lstring.hpp"
#include "../../Lib/IO.hpp"
#include "../../Lib/MimeTypes.hpp"
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
#include "../../HTTPError/HTTPError.hpp"
#include "../../ConfigHandler/Headers/ConfigHandler.hpp"
#include "../../SSL/OpenSSLLoader.hpp"

#define KB 1024
#define DBLCRLF "\r\n\r\n"
#define CRLF "\r\n"
#define SSTR(x) static_cast<std::ostringstream &>(std::ostringstream() << std::dec << x).str()
#define STREAM_TO_STRING(x) static_cast<std::ostringstream &>(std::ostringstream() << x).str()

typedef std::map<std::string, std::string> HeadersType;

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

enum
{
    OK = 200,
    CREATED = 201,
    NOTFOUND = 404,
    FORBIDDEN = 403
};

typedef struct S_File
{
    int Fd;
    std::string ResourceFileType;
} WBSRVFILE;

typedef struct S_DataPool
{
    ServerClass *ServerConf;
    HeadersType Headers;
    WBSRVFILE File;
    std::string Url;
    std::string Query;
    std::string body;
    std::string Location;
    int ResponseStatus;
    int ResourceType;
    int Method;
} DataPool;

typedef HeadersType::iterator HeadersIterator;

std::string GetHeaderAttr(HeadersType &Headers, std::string name);

#endif