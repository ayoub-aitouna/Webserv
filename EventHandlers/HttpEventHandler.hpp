#ifndef READEVENTHANDLER_HPP
#define READEVENTHANDLER_HPP
#include <vector>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "EventHandler.hpp"
#include "../Proccesers/ResponseBuilder.hpp"
#include "../Proccesers/RequestParser.hpp"
#include <sstream>

/**
 * copies binary data from a buffer into a std::string object
 */
#define CBFTSTR(buffer, bytes_number)                   \
    static_cast<std::ostringstream &>(                  \
        std::ostringstream().write(buffer, bytes_number)) \
        .str()
class Client
{
public:
    struct sockaddr_storage address;
    socklen_t address_len;
};

class HttpEventHandler : public EventHandler
{
public:
    HttpEventHandler(int SocketFd, struct sockaddr_storage address, socklen_t address_len);
    HttpEventHandler();
    virtual ~HttpEventHandler();

public:
    int Read();
    int Write();
    EventHandler *Accept();
    const int &GetSocketFd() const;

private:
    Client client;
    RequestParser request;
    ResponseBuilder *response;
};

#endif