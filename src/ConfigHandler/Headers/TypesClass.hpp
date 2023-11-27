#ifndef TYPESCLASS
#define TYPESCLASS
#include <string>
#include <map>
#include "LocationClass.hpp"
#include "Preprocessor.hpp"

class TypesClass
{
private:
    std::string RawData;
    std::map<std::string, std::string> MimeTypes;
    std::map<std::string, std::string> ReverseMimeTypes;

public:
    TypesClass();
    TypesClass(std::string &RawData);
    void SetRawData(std::string &RawData);
    void Parse();
    void DisplayValues(bool Show);
    std::string GetType(std::string FileExtention);
    std::string GetContentExtention(std::string FileExtention);
    ~TypesClass();
};
std::map<std::string, std::string> ReverseTypesMap(std::map<std::string, std::string> m);

#endif