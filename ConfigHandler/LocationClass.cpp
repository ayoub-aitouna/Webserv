#include "Headers/LocationClass.hpp"

LocationClass::LocationClass(std::string &RawData) : RawData(RawData)
{
    Parse();
}

LocationClass::LocationClass(const LocationClass &locatin)
{
    if (this != &locatin)
    {
        this->path = locatin.path;
        this->root = locatin.root;
        this->index = locatin.index;
        this->redirection = locatin.redirection;
        this->autoindex = locatin.autoindex;
        this->allowed = locatin.allowed;
        this->upload = locatin.upload;
        this->upload_stor = locatin.upload_stor;
        this->error_page = locatin.error_page;
        this->cgi = locatin.cgi;
        this->cgi_path = locatin.cgi_path;
    }
}

void ExactSize(bool cond, std::string Block)
{
    if (cond)
        throw std::runtime_error(Block + " syntax invalide");
}

void LocationClass::Parse()
{
    std::stringstream ss(this->RawData);
    std::string Line;
    std::vector<std::string> tokens;
    while (getline(ss, Line))
    {
        tokens = Lstring::Split(std::string(Line), " ");

        if (tokens.size() == 0)
            continue;
        if (tokens.at(0) == "location")
        {
            ExactSize(tokens.size() != 3, "location");
            this->path = tokens.at(1);
        }
        else if (tokens.at(0) == "root")
        {
            ExactSize(tokens.size() != 2, "location");
            this->root = tokens.at(1);
        }
        else if (tokens.at(0) == "index")
        {
            ExactSize(tokens.size() < 2, "location");
            for (size_t i = 1; i < tokens.size(); i++)
                index.push_back(tokens.at(i));
        }
        else if (tokens.at(0) == "return")
        {
            ExactSize(tokens.size() < 2, "location");
            if (tokens.size() > 2)
            {
                this->redirection.first = atoi(tokens.at(1).c_str());
                if (this->redirection.first < 100 || this->redirection.first > 599)
                    throw std::runtime_error("Invalide return Status");
                this->redirection.second = tokens.at(2);
            }
            else
            {
                this->redirection.first = 301;
                this->redirection.second = tokens.at(1);
            }
        }
        else if (tokens.at(0) == "autoindex")
        {
            ExactSize(tokens.size() < 2, "location");
            if (tokens.at(1) == "on")
                this->autoindex = true;
            else if (tokens.at(1) == "off")
                this->autoindex = true;
            else
                throw std::runtime_error("Invalide autoindex Value");
        }
        else if (tokens.at(0) == "allow")
        {
            ExactSize(tokens.size() < 2, "location");
            for (size_t i = 1; i < tokens.size(); i++)
            {
                tokens.at(i) = Lstring::tolower(tokens.at(i));
                if (tokens.at(i) != "get" && tokens.at(i) != "post" && tokens.at(i) != "delete")
                    throw std::runtime_error("Invalide method");
                allowed.push_back(tokens.at(i));
            }
        }
        else if (tokens.at(0) == "upload")
        {
            ExactSize(tokens.size() < 2, "location");
            if (tokens.at(1) == "on")
                this->upload = true;
            else if (tokens.at(1) == "off")
                this->upload = false;
            else
                throw std::runtime_error("Invalide upload Value");
        }
        else if (tokens.at(0) == "upload_stor")
        {
            ExactSize(tokens.size() < 2, "location");
            this->upload_stor = tokens.at(1);
        }
        else if (tokens.at(0) == "error_page")
        {
            ExactSize(tokens.size() < 3, "location");
            this->error_page[atoi(tokens.at(1).c_str())] = tokens.at(2);
        }
        else if (tokens.at(0) != "}")
            throw std::runtime_error("Invalide token " + tokens.at(0));
    }
    DisplayValues(false);
}

void LocationClass::DisplayValues(bool show)
{
    DEBUGOUT(show, COLORED("\n\n------- LocationClass -------\n\n", Magenta));
    DEBUGOUT(show, COLORED("path " << path, Green));
    DEBUGOUT(show, COLORED("root " << root, Green));
    for (size_t i = 0; i < index.size(); i++)
        DEBUGOUT(show, COLORED("index " << index.at(i), Green));
    DEBUGOUT(show, COLORED("redirection " << redirection.first << " : " << redirection.second, Green));
    DEBUGOUT(show, COLORED("autoindex " << autoindex, Green));
    for (size_t i = 0; i < allowed.size(); i++)
        DEBUGOUT(show, COLORED("allowed " << allowed.at(i), Green));
    DEBUGOUT(show, COLORED("upload " << upload, Green));
    DEBUGOUT(show, COLORED("upload_stor " << upload_stor, Green));
    for (std::map<int, std::string>::iterator i = this->error_page.begin(); i != this->error_page.end(); i++)
        DEBUGOUT(show, COLORED("error_page " << i->first << " : " << i->second, Green));
    DEBUGOUT(show, COLORED("\n\n------- END LocationClass -------\n\n", Magenta));
}

std::string LocationClass::GetPath()
{
    return (this->path);
}
std::string LocationClass::GetRoot()
{
    return (this->root);
}
std::vector<std::string> LocationClass::GetIndex()
{
    return (this->index);
}
std::pair<int, std::string> LocationClass::GetRedirection()
{
    return (this->redirection);
}
bool LocationClass::GetAutoindex()
{
    return (this->autoindex);
}
std::vector<std::string> LocationClass::GetAllowed()
{
    return (this->allowed);
}
bool LocationClass::GetUpload()
{
    return (this->upload);
}
std::string LocationClass::GetUpload_stor()
{
    return (this->upload_stor);
}
std::string LocationClass::GetError_page(int ErrorCode)
{
    std::map<int, std::string>::iterator it;
    it = this->error_page.find(ErrorCode);
    if (it != this->error_page.end())
        return (it->second);
    return ("");
}
std::string LocationClass::GetCgi()
{
    return (this->cgi);
}
std::string LocationClass::GetCgi_path()
{
    return (this->cgi_path);
}

LocationClass::~LocationClass()
{
}
