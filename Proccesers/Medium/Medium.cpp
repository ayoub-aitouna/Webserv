#include "Medium.hpp"

Medium::Medium(ResourceFile File, int ResponseBuilderStatus)
{
    this->File = File;
    this->ResponseBuilderStatus = ResponseBuilderStatus;
}

Medium::Medium(int ResponseBuilderStatus)
{
    this->File.Fd = -1;
    this->ResponseBuilderStatus = ResponseBuilderStatus;
}

Medium::Medium()
{
}

Medium::~Medium()
{
}
