#include "Headers/TypesClass.hpp"

TypesClass::TypesClass()
{
}

TypesClass::TypesClass(std::string &RawData) : RawData(RawData)
{
}

void TypesClass::SetRawData(std::string &RawData)
{
    this->RawData = RawData;
}

void TypesClass::Parse()
{
    
}

TypesClass::~TypesClass()
{
}
