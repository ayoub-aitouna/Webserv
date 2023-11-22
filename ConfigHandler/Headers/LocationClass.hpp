#ifndef LOCATIONCLASS
#define LOCATIONCLASS

#include <string>
#include <vector>

class LocationClass
{

public:
    LocationClass(/* args */);
    ~LocationClass();

private:
    std::string path, root;
    std::vector<std::string> index;
    std::pair<int, std::string> redirection;
    std::vector<std::string> allowed;
    bool upload;
    std::string upload_stor;
    std::vector<std::pair<int, std::string> > error_page;
    std::string cgi;
    std::string cgi_path;
};
#endif