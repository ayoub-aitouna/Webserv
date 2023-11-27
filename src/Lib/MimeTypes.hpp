#ifndef MimeTypes_HPP
#define MimeTypes_HPP

#include <string>
#include <map>
#include <sstream>

class MimeTypes
{
private:
    MimeTypes();

public:
    static std::map<std::string, std::string> GetContenTypes(void);
    static std::map<std::string, std::string> GetReverseContenTypes(std::map<std::string, std::string> m);
};

#endif
