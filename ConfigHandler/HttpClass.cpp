#include "Headers/HttpClass.hpp"

HttpClass::HttpClass(std::string &RawData) : RawData(RawData)
{
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
        servers.push_back(ServerClass(Blocks.at(i)));

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
            this->error_page.push_back(std::make_pair(atoi(tokens.at(1).c_str()), tokens.at(2)));
        }
        else if (tokens.at(0) != "}" && tokens.at(0) != "http")
            throw std::runtime_error("Invalide token " + tokens.at(0));
    }

    DEBUGOUT(1, "\n\n-- :: HTTP ::-----\n\n"
                    << COLORED(this->RawData
                                   << "\n\n--------\n\n",
                               Magenta));
    /*Print Resulting Values*/
    DEBUGOUT(1, COLORED("default_type " << this->default_type, Magenta));
    DEBUGOUT(1, COLORED("client_max_body_size " << this->client_max_body_size, Magenta));
    for (size_t i = 1; i < error_page.size(); i++)
        DEBUGOUT(1, COLORED("error_page " << error_page.at(i).first << " : " << error_page.at(i).second, Magenta));
    DEBUGOUT(1, COLORED("\n\n-- :: END HTTP ::-----\n\n",
                        Magenta));
}

HttpClass::~HttpClass()
{
}
