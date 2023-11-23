#ifndef LOCATIONCLASS
#define LOCATIONCLASS

#include <string>
#include <vector>
#include "Preprocessor.hpp"
#include <map>

class LocationClass
{

public:
    LocationClass(std::string &RawData);
    LocationClass(const LocationClass &locatin);
    void Parse();
    void DisplayValues(bool Show);
    std::string &GetPath();
    ~LocationClass();

private:
    std::string RawData;
    std::string path, root;
    std::vector<std::string> index;
    std::pair<int, std::string> redirection;
    bool autoindex;
    std::vector<std::string> allowed;
    bool upload;
    std::string upload_stor;
    std::map<int, std::string> error_page;
    std::string cgi;
    std::string cgi_path;
};
void ExactSize(bool cond, std::string Block);
#endif