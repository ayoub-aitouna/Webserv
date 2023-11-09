#include "Response.hpp"

Response::Response(Request request) : Buffer(""), bufferState(Uncomplete), resposeState(Headers), request(request)
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
    this->bufferState = Ready;
}

Response::~Response()
{
}

BufferState Response::GetBufferState()
{
    return (this->bufferState);
}

int Response::FlushBuffer(int SocketFd)
{
    int s = 1;
    if (this->bufferState == Ready)
    {
        std::cout << Lstring::Colored(this->Buffer, Yellow) << std::endl;
        send(SocketFd, this->Buffer.c_str(), this->Buffer.size(), 0);
        if (this->Buffer == "0\r\n\r\n")
            s = 0;
        this->Buffer.clear();
        if (this->resposeState == Headers)
            this->resposeState = Body;
        bufferState = Uncomplete;
    }
    return s;
}

void Response::FillBuffer()
{
    std::cout << Lstring::Colored("FillBuffer called ", Magenta) << std::endl;

    char buffer[1025];
    memset(buffer, 0, sizeof(buffer));
    std::stringstream ss;
    int BytesCount;

    BytesCount = read(this->File, buffer, KB);
    std::cout << Lstring::Colored(" BytesCount is : ", Magenta) << BytesCount << std::endl;

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
    this->bufferState = Ready;
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