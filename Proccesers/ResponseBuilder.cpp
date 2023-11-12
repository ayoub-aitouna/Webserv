#include "ResponseBuilder.hpp"

void ResponseBuilder::InitStatusCode()
{
    StatusCodes[100] = "Continue";
    StatusCodes[101] = "Switching Protocols";
    StatusCodes[200] = "OK";
    StatusCodes[201] = "Created";
    StatusCodes[202] = "Accepted";
    StatusCodes[203] = "Non-Authoritative Information";
    StatusCodes[204] = "No Content";
    StatusCodes[205] = "Reset Content";
    StatusCodes[206] = "Partial Content";
    StatusCodes[300] = "Multiple Choices";
    StatusCodes[301] = "Moved Permanently";
    StatusCodes[302] = "Found";
    StatusCodes[303] = "See Other";
    StatusCodes[304] = "Not Modified";
    StatusCodes[305] = "Use Proxy";
    StatusCodes[307] = "Temporary Redirect";
    StatusCodes[400] = "Bad RequestParser";
    StatusCodes[401] = "Unauthorized";
    StatusCodes[402] = "Payment Required";
    StatusCodes[403] = "Forbidden";
    StatusCodes[404] = "Not Found";
    StatusCodes[405] = "Method Not Allowed";
    StatusCodes[406] = "Not Acceptable";
    StatusCodes[407] = "Proxy Authentication Required";
    StatusCodes[408] = "RequestParser Timeout";
    StatusCodes[409] = "Conflict";
    StatusCodes[410] = "Gone";
    StatusCodes[411] = "Length Required";
    StatusCodes[412] = "Precondition Failed";
    StatusCodes[413] = "Payload Too Large";
    StatusCodes[414] = "URI Too Long";
    StatusCodes[415] = "Unsupported Media Type";
    StatusCodes[416] = "Range Not Satisfiable";
    StatusCodes[417] = "Expectation Failed";
    StatusCodes[426] = "Upgrade Required";
    StatusCodes[500] = "Internal Server Error";
    StatusCodes[501] = "Not Implemented";
    StatusCodes[502] = "Bad Gateway";
    StatusCodes[503] = "Service Unavailable";
    StatusCodes[504] = "Gateway Timeout";
    StatusCodes[505] = "HTTP Version Not Supported";
}

ResponseBuilder::ResponseBuilder(Medium medium)
{
    this->medium = medium;
    InitStatusCode();
    if (medium.GetFile().Fd != -2)
        FillHeaders(medium.GetResponseStatus());
}

void ResponseBuilder::FillHeaders(int StatusCode)
{
    Buffer = ("HTTP/1.1 " + SSTR(StatusCode) + " " + StatusCodes[StatusCode] + "  \r\n");
    if (medium.GetFile().Fd != -1)
        Buffer += "Content-Type: " + this->medium.GetFile().ResourceFileType + "  \r\n";
    Buffer += "Connection: closed\r\n";
    if (medium.GetFile().Fd != -1)
        Buffer += "Transfer-Encoding: chunked\r\n\r\n";
}

ResponseBuilder::~ResponseBuilder()
{
}

int ResponseBuilder::FlushBuffer(int SocketFd)
{
    if (this->Buffer.empty())
        return (1);
    DEBUGOUT(0, COLORED(this->Buffer, Yellow));
    if (send(SocketFd, this->Buffer.c_str(), this->Buffer.size(), 0) < 0 || this->Buffer == "0\r\n\r\n" || this->medium.GetResponseStatus() != 200)
        return (0);
    this->Buffer.clear();
    this->FillBuffer();
    return (1);
}

void ResponseBuilder::FillBuffer()
{
    char buffer[1025];
    memset(buffer, 0, sizeof(buffer));
    std::stringstream ss;
    int BytesCount;

    BytesCount = read(medium.GetFile().Fd, buffer, KB);
    if (BytesCount < 0)
        throw std::runtime_error("Error Reading ResourceFile");
    else if (BytesCount == 0)
        this->Buffer.append(SSTR(0) + "\r\n\r\n");
    else
    {
        ss << std::hex << BytesCount << "\r\n";
        ss.write(buffer, BytesCount);
        ss << "\r\n";
        this->Buffer = ss.str();
    }
    if (BytesCount < 1)
        close(medium.GetFile().Fd);
}
