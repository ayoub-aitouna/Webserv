#include "Headers/HttpClass.hpp"

HttpClass::HttpClass()
{
}

void HttpClass::SetRawData(std::string &RawData)
{
    this->RawData = RawData;
    Parse();
}

unsigned long ConvertToBytes(std::string &value)
{
    (void)value;
    return (0);
}

void HttpClass::Parse()
{
    std::vector<std::string> Blocks;

    Blocks = ExtractBlock(this->RawData, "types");
    if (Blocks.size() != 1)
        throw std::runtime_error("more or less then 1 `types` Block");
    this->types.SetRawData(Blocks.at(0));
    this->types.Parse();
    Blocks = ExtractBlock(this->RawData, "server");
    if (!Blocks.size())
        throw std::runtime_error("It Should be at least one `server` Block");
    for (size_t i = 0; i < Blocks.size(); i++)
    {
        DEBUGOUT(1, "HEEEEY-------");
        servers.push_back(ServerClass(Blocks.at(i)));
    }

    /**
     * PARSE http block props
     */
    std::stringstream ss(this->RawData);
    std::string Line;
    std::vector<std::string> tokens;
    while (getline(ss, Line))
    {
        tokens = Lstring::Split(std::string(Line), " ");

        if (tokens.size() == 0)
            continue;
        if (tokens.at(0) == "default_type")
        {
            ExactSize(tokens.size() != 2, "HTTP");
            this->default_type = tokens.at(1);
        }
        else if (tokens.at(0) == "client_max_body_size")
        {
            ExactSize(tokens.size() != 2, "HTTP");
            this->client_max_body_size = ConvertToBytes(tokens.at(1));
        }
        else if (tokens.at(0) == "error_page")
        {
            ExactSize(tokens.size() < 3, "HTTP");
            this->error_page[atoi(tokens.at(1).c_str())] = tokens.at(2);
        }
        else if (tokens.at(0) != "}" && tokens.at(0) != "http")
            throw std::runtime_error("Invalide token " + tokens.at(0));
    }
    DisplayValues(false);
}

std::string HttpClass::GetContentType(std::string FileExtention)
{
    std::string Type;
    Type = this->types.GetType(FileExtention);
    if (Type.empty())
        return (this->default_type);
    return (Type);
}

std::string HttpClass::GetFileExtention(std::string ContentType)
{
    return (this->types.GetContentExtention(ContentType));
}

std::string HttpClass::GetGlobalErrorPagePath(int ErrorCode)
{
    std::map<int, std::string>::iterator it;
    it = this->error_page.find(ErrorCode);
    if (it != this->error_page.end())
        return (it->second);
    return ("");
}

unsigned long HttpClass::GetMaxBodySize()
{
    return (this->client_max_body_size);
}

std::vector<ServerClass> &HttpClass::GetServers()
{
    return (this->servers);
}

ServerClass *HttpClass::GetServersByHost(std::string host)
{

    std::string hostname;
    for (size_t i = 0; i < this->servers.size(); i++)
    {
        hostname = this->servers.at(i).GetHostName() + ":" + this->servers.at(i).GetPort();
        if (host == hostname)
            return (new ServerClass(this->servers.at(i)));
    }

    if (!this->servers.empty())
        return (new ServerClass(this->servers.at(0)));
    return (NULL);
}

void HttpClass::DisplayValues(bool show)
{
    DEBUGOUT(show, COLORED("\n\n-- :: HTTP ::-----\n\n", Magenta));
    DEBUGOUT(show, COLORED("default_type " << this->default_type, Magenta));
    DEBUGOUT(show, COLORED("client_max_body_size " << this->client_max_body_size, Magenta));
    for (std::map<int, std::string>::iterator i = this->error_page.begin(); i != this->error_page.end(); i++)
        DEBUGOUT(show, COLORED("error_page " << i->first << " : " << i->second, Magenta));
    DEBUGOUT(show, COLORED("\n\n-- :: END HTTP ::-----\n\n", Magenta));
}

HttpClass::~HttpClass()
{
}
