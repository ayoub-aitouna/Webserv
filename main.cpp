#include "Server/Server.hpp"
#include <vector>
#include "EventHandlers/HttpEventHandler.hpp"
#include <signal.h>

void HandleSigPip(int sinal)
{
    char *msg = (char *)"SigPip\n";
    write(1, msg, strlen(msg));
}

int main()
{
    signal(SIGPIPE, HandleSigPip);
    Server server("127.0.0.1", "8080");
    try
    {
        server.CreatSocket();
        server.Run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}