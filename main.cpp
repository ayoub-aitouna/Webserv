#include "Server/Server.hpp"
#include <vector>
#include "EventHandlers/HttpEventHandler.hpp"
#include <signal.h>

void HandleSigPip(int signal)
{

    std::string msg = "SigPip " + SSTR(signal) + "\n";
    write(1, msg.c_str(), msg.size());
}

int main()
{
    signal(SIGPIPE, HandleSigPip);
    std::string str(CBFTSTR("aaa", 3));
    
    Server server("", "8080");
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