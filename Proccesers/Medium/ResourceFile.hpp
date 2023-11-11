#ifndef RESOURCEFILE
#define RESOURCEFILE

#include <string>
class ResourceFile
{

public:
    ResourceFile(int fd);
    ResourceFile();
    int Fd;
    std::string ResourceFileType;
};

#endif
