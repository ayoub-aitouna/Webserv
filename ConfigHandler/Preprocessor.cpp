
#include "Headers/Preprocessor.hpp"

Preprocessor::Preprocessor(std::string &data)
{
    std::stringstream ss(data);
    std::ostringstream ProcessedData;
    std::string Line;
    std::vector<std::string> LineParts;
    while (std::getline(ss, Line))
    {
        // Ignore comments
        Lstring::Trim(Line);
        if (Line.size() == 0)
            continue;
        if (Line.at(0) == '#')
            continue;
        if (Line.find("#") != std::string::npos)
            Line.erase(Line.find("#"));
        Lstring::Trim(Line);
        if (*(Line.end() - 1) != ';' && *(Line.end() - 1) != '{' && *(Line.end() - 1) != '}')
        {
            DEBUGOUT(1, COLORED("CONFIG FILE ERROR \n Line : " << Line, Red));
            throw std::runtime_error("Conf file invalide \nLine : \n\t" + Line);
        }
        if (*(Line.end() - 1) == ';')
            Line.erase((Line.end() - 1));
        LineParts = Lstring::Split(Line, " ");
        if (LineParts.size() >= 2)
        {
            if (LineParts.at(0) == "include")
            {
                std::ifstream includeFile(LineParts.at(1).c_str());
                if (!includeFile)
                    throw std::runtime_error("Invalide included File " + LineParts.at(1));
                ss << includeFile.rdbuf();
                continue;
            }
        }
        ProcessedData << Line << '\n';
    }

    std::cout << "\n\n\n"
              << ProcessedData.str()
              << "\n\n\n"
              << std::endl;
}

Preprocessor::~Preprocessor()
{
}
