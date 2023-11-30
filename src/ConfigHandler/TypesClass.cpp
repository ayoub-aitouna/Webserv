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
    Parse();
}

void TypesClass::Parse()
{
    std::stringstream ss(this->RawData);
    std::string Line;
    std::vector<std::string> tokens;

    while (getline(ss, Line))
    {
        tokens = Lstring::SplitByOneOf(std::string(Line), " \t");
        if (tokens.size() == 0 || tokens.at(0) == "}" || tokens.at(0) == "types")
            continue;
        ExactSize(tokens.size() < 2, "Types");
        for (size_t i = 1; i < tokens.size(); i++)
            this->MimeTypes[tokens.at(i)] = tokens.at(0);
    }
    this->ReverseMimeTypes = ReverseTypesMap(this->MimeTypes);
    DisplayValues(false);
}

void TypesClass::DisplayValues(bool Show)
{
    DEBUGOUT(Show, COLORED("\n\n------- TypesClass -------\n\n", Magenta));
    for (std::map<std::string, std::string>::iterator it = this->MimeTypes.begin();
         it != this->MimeTypes.end(); it++)
        DEBUGOUT(Show, COLORED(it->first, Magenta) << COLORED(" : " << it->second, Green));
    DEBUGOUT(Show, COLORED("\n\n------- END TypesClass -------\n\n", Magenta));
}

std::string TypesClass::GetType(std::string FileExtention)
{
    return this->MimeTypes[FileExtention];
}

std::string TypesClass::GetContentExtention(std::string contentType)
{
    return (this->ReverseMimeTypes[contentType]);
}

TypesClass::~TypesClass()
{
}

std::map<std::string, std::string> ReverseTypesMap(std::map<std::string, std::string> m)
{
    std::map<std::string, std::string> r;
    for (std::map<std::string, std::string>::iterator i = m.begin(); i != m.end(); i++)
        r[i->second] = i->first;
    return r;
}