#ifndef MEDIUM
#define MEDIUM

#include <string>
#include "ResourceFile.hpp"

class Medium
{

public:
    Medium(ResourceFile File, int ResponseBuilderStatus);
    Medium(int ResponseBuilderStatus);
    Medium();
    ~Medium();

public:
    ResourceFile File;
    int ResponseBuilderStatus;
};

#endif
