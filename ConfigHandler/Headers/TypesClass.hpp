#ifndef TYPESCLASS
#define TYPESCLASS
#include <string>
#include <map>

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
    ~TypesClass();
};

#endif