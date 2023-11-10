#include "Response.hpp"

Response::Response(Request request) : Buffer(""), request(request)
{
    Content_Types = MimeTypes::GetContenTypes();
    GetFilePath();
    FillHeaders(200);
}

void Response::FillHeaders(int StatusCode)
{

    Buffer = "HTTP/1.1 200 OK \r\n";
    Buffer += ("Content-Type: " + this->FileType + " \r\n");
    Buffer += "Connection: closed\r\n";
    Buffer += "Transfer-Encoding: chunked\r\n\r\n";
}

Response::~Response()
{
}

int Response::FlushBuffer(int SocketFd)
{
    // std::cout << Lstring::Colored(this->Buffer, Yellow) << std::endl;
    if (send(SocketFd, this->Buffer.c_str(), this->Buffer.size(), 0) < 0 || this->Buffer == "0\r\n\r\n")
        return (0);
    this->Buffer.clear();
    this->FillBuffer();
    return (1);
}

void Response::FillBuffer()
{
    char buffer[1025];
    memset(buffer, 0, sizeof(buffer));
    std::stringstream ss;
    int BytesCount;

    BytesCount = read(this->File, buffer, KB);
    if (BytesCount < 0)
        throw std::runtime_error("Error Reading File");
    else if (BytesCount == 0)
        this->Buffer.append(std::to_string(0) + "\r\n\r\n");
    else
    {
        ss << std::hex << BytesCount << "\r\n";
        ss.write(buffer, BytesCount);
        ss << "\r\n";
        this->Buffer = ss.str();
    }
    if (BytesCount < 1)
        close(this->File);
}

void Response::GetFilePath()
{
    std::string Url = request.GetUrl();
    std::string FilePath;
    size_t LastDotPos;
    if (Url.find("..") != std::string::npos)
        throw std::runtime_error("Invalide Url");
    FilePath = "public" + Url;
    if (Url == "/" || Url.back() == '/')
        FilePath += "index.html";
    std::cout << Lstring::Colored("File Path ", Cyan) + Lstring::Colored(FilePath, Green) << std::endl;
    if ((LastDotPos = FilePath.find_last_of(".")) == std::string::npos)
        throw std::runtime_error("Invalide File Name");
    this->FileType = this->Content_Types[FilePath.substr(LastDotPos)];
    this->File = open(FilePath.c_str(), O_RDONLY);
    if (this->File < 0)
        throw std::runtime_error("Invalide File");
}