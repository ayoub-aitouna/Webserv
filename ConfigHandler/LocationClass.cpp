#include "Headers/LocationClass.hpp"

LocationClass::LocationClass(std::string &RawData) : RawData(RawData)
{
    Parse();
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
            this->error_page.push_back(std::make_pair(atoi(tokens.at(1).c_str()), tokens.at(2)));
        }
        else if (tokens.at(0) != "}")
            throw std::runtime_error("Invalide token " + tokens.at(0));
    }

    DEBUGOUT(1, "\n\n--------\n\n"
                    << COLORED(this->RawData
                                   << "\n\n--------\n\n",
                               Magenta));

    /*Print Resulting Values*/
    DEBUGOUT(1, COLORED("path " << path, Green));
    DEBUGOUT(1, COLORED("root " << root, Green));
    for (size_t i = 0; i < index.size(); i++)
        DEBUGOUT(1, COLORED("index " << index.at(i), Green));
    DEBUGOUT(1, COLORED("redirection " << redirection.first << " : " << redirection.second, Green));
    DEBUGOUT(1, COLORED("autoindex " << autoindex, Green));
    for (size_t i = 0; i < allowed.size(); i++)
        DEBUGOUT(1, COLORED("allowed " << allowed.at(i), Green));
    DEBUGOUT(1, COLORED("upload " << upload, Green));
    DEBUGOUT(1, COLORED("upload_stor " << upload_stor, Green));
    for (size_t i = 0; i < error_page.size(); i++)
        DEBUGOUT(1, COLORED("error_page " << error_page.at(i).first << " : " << error_page.at(i).second, Green));
}

LocationClass::~LocationClass()
{
}
