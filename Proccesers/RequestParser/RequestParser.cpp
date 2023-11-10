#include "RequestParser.hpp"

RequestParser::RequestParser()
{
    Content_Types = MimeTypes::GetContenTypes();
}

void RequestParser::UriDecode()
{
}

RequestParser::~RequestParser()
{
}

File RequestParser::GetFilePath()
{
    File file;
    std::string FilePath;
    size_t LastDotPos;

    if (this->url.find("..") != std::string::npos)
        throw std::runtime_error("Invalide Url");
    FilePath = "public" + this->url;
    if (this->url == "/" || this->url.back() == '/')
        FilePath += "index.html";
    if ((LastDotPos = FilePath.find_last_of(".")) == std::string::npos)
        throw std::runtime_error("Invalide File Name");
    file.FileType = this->Content_Types[FilePath.substr(LastDotPos)];
    file.File = open(FilePath.c_str(), O_RDONLY);
    if (file.File < 0)
        throw std::runtime_error("Invalide File");

    std::cout << Lstring::Colored("File Path ", Cyan) + Lstring::Colored(FilePath, Green) << std::endl;
    return (file);
}