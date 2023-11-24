#include "Headers/ServerClass.hpp"

/**
 * Constructors
 */
ServerClass::ServerClass()
{
    this->location = NULL;
}

ServerClass::ServerClass(std::string &RawData) : RawData(RawData)
{
    this->location = NULL;
    Parse();
}

ServerClass::ServerClass(const ServerClass &lhs)
{
    *this = lhs;
}

void ServerClass::SetData(std::string &RawData)
{
    this->RawData = RawData;
    Parse();
}

ServerClass &ServerClass::operator=(const ServerClass &lhs)
{
    if (this != &lhs)
    {
        this->RawData = lhs.RawData;
        this->port = lhs.port;
        this->host = lhs.host;
        this->root = lhs.root;
        this->server_name = lhs.server_name;
        this->redirection = lhs.redirection;
        this->error_page = lhs.error_page;
        this->index = lhs.index;
        this->locations = lhs.locations;
    }
    return (*this);
}

/**
 * PARSERS
 */

void ServerClass::Parse()
{
    std::vector<std::string> Blocks;

    Blocks = ExtractBlock(this->RawData, "location");
    for (size_t i = 0; i < Blocks.size(); i++)
        locations.push_back(LocationClass(Blocks.at(i)));

    std::stringstream ss(this->RawData);
    std::string Line;
    std::vector<std::string> tokens;

    while (getline(ss, Line))
    {
        tokens = Lstring::Split(std::string(Line), " ");

        if (tokens.size() == 0)
            continue;
        if (tokens.at(0) == "listen")
        {
            ExactSize(tokens.size() != 2, "Server");
            this->port = tokens.at(1);
        }
        else if (tokens.at(0) == "host")
        {
            ExactSize(tokens.size() != 2, "Server");
            this->host = tokens.at(1);
        }
        else if (tokens.at(0) == "server_name")
        {
            ExactSize(tokens.size() < 2, "Server");
            for (size_t i = 1; i < tokens.size(); i++)
                server_name.push_back(tokens.at(i));
        }
        else if (tokens.at(0) == "index")
        {
            ExactSize(tokens.size() < 2, "Server");
            for (size_t i = 1; i < tokens.size(); i++)
                this->index.push_back(tokens.at(i));
        }
        else if (tokens.at(0) == "root")
        {
            ExactSize(tokens.size() != 2, "Server");
            this->root = tokens.at(1);
        }
        else if (tokens.at(0) == "return")
        {
            ExactSize(tokens.size() < 2, "Server");
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
        else if (tokens.at(0) == "error_page")
        {
            ExactSize(tokens.size() < 3, "Server");
            this->error_page[atoi(tokens.at(1).c_str())] = tokens.at(2);
        }
        else if (tokens.at(0) != "}" && tokens.at(0) != "server")
            throw std::runtime_error("Invalide token " + tokens.at(0));
    }
    DisplayValues(false);
}

void ServerClass::DisplayValues(bool Show)
{
    DEBUGOUT(Show, COLORED("\n\n------- ServerClass -------\n\n", Magenta));
    DEBUGOUT(Show, COLORED("port " << this->port, Magenta));
    DEBUGOUT(Show, COLORED("host " << host, Magenta));
    for (size_t i = 0; i < server_name.size(); i++)
        DEBUGOUT(Show, COLORED("server_name " << server_name.at(i), Magenta));
    DEBUGOUT(Show, COLORED("root " << root, Magenta));
    DEBUGOUT(Show, COLORED("redirection " << redirection.first << " : " << redirection.second, Magenta));
    for (std::map<int, std::string>::iterator i = this->error_page.begin(); i != this->error_page.end(); i++)
        DEBUGOUT(Show, COLORED("error_page " << i->first << " : " << i->second, Magenta));

    DEBUGOUT(Show, "Locations :");
    for (size_t i = 0; i < this->locations.size(); i++)
        DEBUGOUT(Show, COLORED(this->locations.at(i).GetPath(), Magenta));

    DEBUGOUT(Show, COLORED("\n\n-- :: END SERVER ::-----\n\n", Magenta));
}

/**
 * GETTERS
 */
std::string ServerClass::GetErrorPagePath(int ErrorCode)
{
    std::map<int, std::string>::iterator it;
    it = this->error_page.find(ErrorCode);
    if (it != this->error_page.end())
        return (it->second);
    return ("");
}

std::string ServerClass::GetHostName()
{
    return (this->host);
}

std::string ServerClass::GetPort()
{
    return (this->port);
}

std::string ServerClass::GetRoot(std::string &Url)
{
    if (location && !location->GetRoot().empty())
        return Lstring::Replace(Url, location->GetPath(), location->GetRoot());
    return (Lstring::RTrim(root, "/") + Url);
}

std::vector<std::string> &ServerClass::GetServerNames()
{
    return (this->server_name);
}

std::vector<std::string> ServerClass::GetIndex()
{
    if (location != NULL && location->GetIndex().size() > 0)
        return (location->GetIndex());
    else
        return (index);
}

std::pair<int, std::string> ServerClass::GetRedirection(std::string &path)
{

    LocationClass *location;
    std::pair<int, std::string> redirection;

    redirection = this->redirection;
    location = NULL;
    location = GetLocation(path);

    if (!location)
        return (redirection);
    if (location->GetRedirection().first != 0)
        redirection = location->GetRedirection();
    return (delete location, redirection);
}

void ServerClass::SetRequestPath(std::string &path)
{
    location = GetLocation(path);
    if (location)
        location->DisplayValues(true);
    else
        DEBUGOUT(1, "LOCATION NOT FOUND");
}

LocationClass *ServerClass::GetLocation(std::string &path)
{
    LocationClass *lastmatch = NULL;
    size_t matchSize = 0, locationPathSize;
    for (size_t i = 0; i < this->locations.size(); i++)
    {
        locationPathSize = this->locations.at(i).GetPath().length();
        if (locationPathSize > matchSize &&
            path.find(this->locations.at(i).GetPath().c_str(), 0, locationPathSize) != std::string::npos)
        {
            lastmatch = new LocationClass(this->locations.at(i));
            matchSize = locationPathSize;
        }
    }
    return (lastmatch);
}

ServerClass::~ServerClass()
{
}
