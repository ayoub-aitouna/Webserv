#include "Headers/ServerClass.hpp"

// ? ********** Constructors *************
ServerClass::ServerClass()
{
    this->location = NULL;
    this->SSlOn = false;
}

ServerClass::ServerClass(std::string &RawData) : RawData(RawData)
{
    this->location = NULL;
    this->SSlOn = false;

    Parse();
}

ServerClass::ServerClass(const ServerClass &lhs)
{
    this->location = NULL;
    this->SSlOn = false;
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
        this->server_names = lhs.server_names;
        this->redirection = lhs.redirection;
        this->error_page = lhs.error_page;
        this->index = lhs.index;
        this->locations = lhs.locations;
        this->SSlOn = lhs.SSlOn;
        this->SSlCertificate = lhs.SSlCertificate;
        this->SSlcertificatekey = lhs.SSlcertificatekey;

        this->autoindex = lhs.autoindex;
        this->upload = lhs.upload;
        this->upload_stor = lhs.upload_stor;
        this->cgi = lhs.cgi;
        this->cgi_path = lhs.cgi_path;
    }
    return (*this);
}
// ? ********** END Constructors *************

// ? SERVER CLLASS PARSER FUNCTIONS
void ServerClass::Parse()
{
    std::vector<std::string> tokens;
    std::vector<std::string> Blocks;
    std::string Line;
    int port;

    Blocks = ExtractBlock(this->RawData, "location");
    for (size_t i = 0; i < Blocks.size(); i++)
        locations.push_back(LocationClass(Blocks.at(i)));

    std::stringstream ss(this->RawData);

    getline(ss, Line);
    while (getline(ss, Line))
    {
        tokens = Lstring::SplitByOneOf(std::string(Line), " \t");

        if (tokens.size() == 0)
            continue;
        if (tokens.at(0) == "listen")
        {
            ExactSize(tokens.size() < 2 || tokens.size() > 3, "Server");
            for (size_t i = 0; i < tokens.at(1).size(); i++)
            {
                if (!isdigit(tokens[1][i]))
                    throw std::runtime_error("Invalide Value of `listen` Incorrect");
            }
            this->port = tokens.at(1);
            port = atoi(this->port.c_str());
            if (port < 0 || port > 65535)
                throw std::runtime_error("Invalide Value of `listen` OutOfBounds");
            if (tokens.size() == 3)
            {
                if (tokens.at(2) != "ssl")
                    throw std::runtime_error("Invalide Mode :  " + tokens.at(2));
                this->SSlOn = true;
            }
        }
        else if (tokens.at(0) == "host")
        {
            ExactSize(tokens.size() != 2, "Server");
            this->host = tokens.at(1);
        }
        else if (tokens.at(0) == "server_names")
        {
            ExactSize(tokens.size() < 2, "Server");
            for (size_t i = 1; i < tokens.size(); i++)
                server_names.push_back(tokens.at(i));
        }
        else if (tokens.at(0) == "ssl_certificate")
        {
            ExactSize(tokens.size() != 2, "Server");
            this->SSlCertificate = tokens.at(1);
        }
        else if (tokens.at(0) == "ssl_certificate_key")
        {
            ExactSize(tokens.size() != 2, "Server");
            this->SSlcertificatekey = tokens.at(1);
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
                for (size_t i = 0; i < tokens.at(1).size(); i++)
                {
                    if (!isdigit(tokens[1][i]))
                        throw std::runtime_error("Invalide Value of `return` Incorrect");
                }
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
            for (size_t i = 0; i < tokens.at(1).size(); i++)
            {
                if (!isdigit(tokens[1][i]))
                    throw std::runtime_error("Invalide Value of `error_page` Incorrect");
            }
            this->error_page[atoi(tokens.at(1).c_str())] = tokens.at(2);
        }
        else if (tokens.at(0) == "autoindex")
        {
            ExactSize(tokens.size() < 2, "location");
            this->autoindex = tokens.at(1);
            Lstring::tolower(this->autoindex);
            if (this->autoindex != "on" && this->autoindex != "off")
                throw std::runtime_error("Invalide autoindex Value");
        }
        else if (tokens.at(0) == "upload")
        {
            ExactSize(tokens.size() < 2, "location");

            this->upload = tokens.at(1);
            Lstring::tolower(this->upload);
            if (this->upload != "on" && this->upload != "off")
                throw std::runtime_error("Invalide upload Value");
        }
        else if (tokens.at(0) == "upload_stor")
        {
            ExactSize(tokens.size() < 2, "Server");
            this->upload_stor = tokens.at(1);
        }
        else if (tokens.at(0) == "cgi_accept")
        {
            ExactSize(tokens.size() < 2, "Server");
            for (size_t i = 1; i < tokens.size(); i++)
                cgi.push_back(tokens.at(i));
        }
        else if (tokens.at(0) == "cgi_path")
        {
            ExactSize(tokens.size() != 2, "Server");
            this->cgi_path = tokens.at(1);
        }

        else if (tokens.at(0) != "}")
            throw std::runtime_error("Invalide token `" + tokens.at(0) + "`");
    }
    DisplayValues(0);
    Validate_Values();
}

void ServerClass::DisplayValues(bool Show)
{
    DEBUGOUT(Show, COLORED("\n\n------- ServerClass -------\n\n", Magenta));
    DEBUGOUT(Show, COLORED("port " << this->port, Magenta));
    DEBUGOUT(Show, COLORED("host " << host, Magenta));
    for (size_t i = 0; i < server_names.size(); i++)
        DEBUGOUT(Show, COLORED("server_names " << server_names.at(i), Magenta));
    DEBUGOUT(Show, COLORED("root " << root, Magenta));
    DEBUGOUT(Show, COLORED("redirection " << redirection.first << " : " << redirection.second, Magenta));
    for (std::map<int, std::string>::iterator i = this->error_page.begin(); i != this->error_page.end(); i++)
        DEBUGOUT(Show, COLORED("error_page " << i->first << " : " << i->second, Magenta));

    DEBUGOUT(Show, "Locations :");
    for (size_t i = 0; i < this->locations.size(); i++)
        DEBUGOUT(Show, COLORED(this->locations.at(i).GetPath(), Magenta));

    DEBUGOUT(Show, COLORED("\n\n-- :: END SERVER ::-----\n\n", Magenta));
}

void ServerClass::Validate_Values()
{
    if (this->host.empty() || this->port.empty())
        throw std::runtime_error("Error: Missing ( Host | Port ) ");
    if (this->root.empty())
    {
        if (this->locations.empty())
            throw std::runtime_error("Error: Missing `root` at Server: " + this->host + ":" + this->port);

        for (size_t i = 0; i < this->locations.size(); i++)
        {
            if (this->locations.at(i).GetRoot().empty())
                throw std::runtime_error("Error: If No `root` Is Specified on Server.\n       All Locations Should Provide their Own `root` At Server: " + this->host + ":" + this->port);
        }
    }
    if (this->SSlOn && (this->SSlCertificate.empty() || this->SSlcertificatekey.empty()))
        throw std::runtime_error("Error: Missing `ssl_certificate Or ssl_certificate_key` at Server: " + this->host + ":" + this->port);

    try
    {
        if (this->SSlOn)
        {
            CheckIfValidePath(this->SSlCertificate, false);
            CheckIfValidePath(this->SSlcertificatekey, false);
        }
        if (IS_ON_OR_OFF(this->upload))
            CheckIfValidePath(this->upload_stor);
        if (!this->cgi.empty())
            CheckIfValidePath(this->cgi_path, false);
        if (!this->root.empty())
            CheckIfValidePath(this->root);
        for (std::map<int, std::string>::iterator i = this->error_page.begin(); i != this->error_page.end(); i++)
            CheckIfValidePath(i->second, false);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("Error: " + std::string(e.what()) + " at Server: " + this->host + ":" + this->port);
    }
}
//? ***********************

// ? ********** GETTERS *************
std::string ServerClass::GetErrorPagePath(int ErrorCode)
{
    std::string Path = this->error_page[ErrorCode];

    if (!Path.empty())
        return (Path);
    if (this->location != NULL)
        return (this->location->GetError_page(ErrorCode));
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

std::string ServerClass::GetRoot(std::string Url)
{
    if (location && !location->GetRoot().empty())
        return Lstring::Replace(Url, location->GetPath(), location->GetRoot());
    return (Lstring::RTrim(root, "/") + Url);
}

std::vector<std::string> &ServerClass::GetServerNames()
{
    return (this->server_names);
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

bool ServerClass::GetAutoindex()
{
    if (this->location && !this->location->GetAutoindex().empty())
        return IS_ON_OR_OFF(this->location->GetAutoindex());
    return IS_ON_OR_OFF(this->autoindex);
}

bool ServerClass::GetUpload()
{
    if (this->location && !this->location->GetUpload().empty())
        return IS_ON_OR_OFF(this->location->GetUpload());
    return IS_ON_OR_OFF(this->upload);
}

std::string ServerClass::GetUpload_stor()
{
    if (location && !location->GetUpload().empty())
        return (this->location->GetUpload_stor());
    return (this->upload_stor);
}

std::vector<std::string> &ServerClass::GetCgi()
{
    if (location && !location->GetCgi().empty())
        return (this->location->GetCgi());
    return (this->cgi);
}

std::vector<std::string> ServerClass::GetAllowed()
{
    if (location)
        return (this->location->GetAllowed());
    return (std::vector<std::string>());
}

std::string ServerClass::GetCgi_path()
{
    if (location && !location->GetCgi_path().empty())
        return (this->location->GetCgi_path());
    return (this->cgi_path);
}

bool ServerClass::IsSSlOn()
{
    return (this->SSlOn);
}

std::string ServerClass::GetSSlCertificate()
{
    return (this->SSlCertificate);
}

std::string ServerClass::GetSSlcertificatekey()
{
    return (this->SSlcertificatekey);
}

void ServerClass::SetRequestPath(std::string &path)
{
    location = GetLocation(path);
    if (location)
        location->DisplayValues(false);
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
// ? ********** END GETTERS *************

ServerClass::~ServerClass()
{
    if (this->location)
        delete this->location;
}
