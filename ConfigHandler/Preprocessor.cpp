
#include "Headers/Preprocessor.hpp"

Preprocessor::Preprocessor(std::ifstream &inputFile) : inputFile(inputFile)
{
}

void CheckBrakets(std::string &ProccesedData)
{
    int brakets = 0;

    for (size_t i = 0; i < ProccesedData.size(); i++)
    {
        if (ProccesedData.at(i) == '{')
            brakets++;
        else if (ProccesedData.at(i) == '}')
            brakets--;
        if (brakets < 0)
            throw std::runtime_error("Invalide Conf File\nReason:\n\tBraket not closed properly");
    }
    if (brakets != 0)
        throw std::runtime_error("Invalide Conf File\nReason:\n\tBraket not (closed or opend) properly");
}

std::string Preprocessor::Processor()
{
    std::string ProccesedData;
    std::stringstream ss;
    std::stringstream Tmpss;
    std::ostringstream ProcessedData;
    std::string Line;
    std::vector<std::string> LineParts;

    ss << inputFile.rdbuf();
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
                Tmpss.str("");
                Tmpss << ss.rdbuf();
                ss.str("");
                ss << includeFile.rdbuf() << '\n'
                   << Tmpss.str();
                continue;
            }
        }
        ProcessedData << Line << '\n';
    }
    ProccesedData = ProcessedData.str();
    CheckBrakets(ProccesedData);
    std::ofstream processedFile("processedFile.conf");
    processedFile << ProccesedData;
    return (ProccesedData);
}

void erase_portion(std::string &data, int start, int end)
{
    std::string tmp = data.substr(0, start);
    data = data.substr(end);
    data = tmp + data;
}

std::vector<std::string> ExtractBlock(std::string &data, std::string BlockName)
{
    size_t start;
    size_t brackets = 0;
    std::vector<std::string> blocks;

    for (;;)
    {
        if ((start = data.find(BlockName)) == std::string::npos)
            break;

        for (size_t index = data.find_first_of('{', start); index < data.size(); index++)
        {

            if (data[index] == '{')
                brackets++;
            else if (data[index] == '}')
                brackets--;
            if (brackets == 0)
            {
                blocks.push_back(data.substr(start, (index - start) + 1));
                erase_portion(data, start, index + 1);
                break;
            }
        }
        if (brackets != 0)
            throw std::runtime_error("Couldn't find proper Block : " + BlockName);
    }
    return (blocks);
}

Preprocessor::~Preprocessor()
{
}
