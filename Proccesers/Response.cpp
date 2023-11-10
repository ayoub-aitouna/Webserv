#include "Response.hpp"

Response::Response(ResourceFile file)
{
    this->file = file;
    FillHeaders(200);
}

void Response::FillHeaders(int StatusCode)
{
    Buffer = "HTTP/1.1 200 OK \r\n";
    Buffer += ("Content-Type: " + this->file.ResourceFileType + " \r\n");
    Buffer += "Connection: closed\r\n";
    Buffer += "Transfer-Encoding: chunked\r\n\r\n";
}

Response::~Response()
{
}

int Response::FlushBuffer(int SocketFd)
{
    if (this->Buffer.empty())
        return (1);
    std::cout << Lstring::Colored(this->Buffer, Yellow) << std::endl;
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

    BytesCount = read(file.Fd, buffer, KB);
    if (BytesCount < 0)
        throw std::runtime_error("Error Reading ResourceFile");
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
        close(file.Fd);
}
