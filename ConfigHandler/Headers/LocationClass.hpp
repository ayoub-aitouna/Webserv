#ifndef LOCATIONCLASS
#define LOCATIONCLASS

#include <string>
#include <vector>
#include "Preprocessor.hpp"
#include <map>

#define IS_ON_OR_OFF(val) val == "on" ? true : false

class LocationClass
{

public:
    LocationClass(std::string &RawData);
    LocationClass(const LocationClass &locatin);
    ~LocationClass();

public:
    void Parse();
    void DisplayValues(bool Show);
    std::string GetPath();
    std::string GetRoot();
    std::vector<std::string> GetIndex();
    std::pair<int, std::string> GetRedirection();
    std::string GetAutoindex();
    std::vector<std::string> GetAllowed();
    std::string GetUpload();
    std::string GetUpload_stor();
    std::string GetError_page(int ErrorCode);
    std::vector<std::string> &GetCgi();
    std::string GetCgi_path();

private:
    std::string RawData;
    std::string path, root;
    std::vector<std::string> index;
    std::pair<int, std::string> redirection;
    std::string autoindex;
    std::vector<std::string> allowed;
    std::string upload;
    std::string upload_stor;
    std::map<int, std::string> error_page;
    std::vector<std::string> cgi;
    std::string cgi_path;
};
void ExactSize(bool cond, std::string Block);
#endif