#include "Headers/ServerClass.hpp"

ServerClass::ServerClass(std::string &RawData) : RawData(RawData)
{
    Parse();
}

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
            this->error_page.push_back(std::make_pair(atoi(tokens.at(1).c_str()), tokens.at(2)));
        }
        else if (tokens.at(0) != "}" && tokens.at(0) != "server")
            throw std::runtime_error("Invalide token " + tokens.at(0));
    }

    DEBUGOUT(1, "\n\n-- :: SERVER ::-----\n\n"
                    << COLORED(this->RawData
                                   << "\n\n--------\n\n",
                               Magenta));
    /*Print Resulting Values*/
    DEBUGOUT(1, COLORED("port " << port, Magenta));
    DEBUGOUT(1, COLORED("host " << host, Magenta));
    for (size_t i = 0; i < server_name.size(); i++)
        DEBUGOUT(1, COLORED("server_name " << server_name.at(i), Magenta));
    DEBUGOUT(1, COLORED("root " << root, Magenta));
    DEBUGOUT(1, COLORED("redirection " << redirection.first << " : " << redirection.second, Magenta));
    for (size_t i = 0; i < error_page.size(); i++)
        DEBUGOUT(1, COLORED("error_page " << error_page.at(i).first << " : " << error_page.at(i).second, Magenta));
    DEBUGOUT(1, COLORED("\n\n-- :: END SERVER ::-----\n\n",
                        Magenta));
}

ServerClass::~ServerClass()
{
}
