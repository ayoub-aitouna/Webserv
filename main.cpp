#include "Server/Server.hpp"
#include <vector>
#include <signal.h>
#include "ConfigHandler/Headers/ConfigHandler.hpp"
void HandleSigPip(int signal)
{

    std::string msg = "SigPip " + SSTR(signal) + "\n";
    write(1, msg.c_str(), msg.size());
}

int main(int ac, char **av)
{
    if (ac > 1)
        ConfigHandler conf(av[1]);
    signal(SIGPIPE, HandleSigPip);
    Server server;
    try
    {
        for (size_t i = 0; i < 10; i++)
            server.CreatSocket("", "80" + SSTR(80 + i));
        server.Run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}