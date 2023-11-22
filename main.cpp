#include "Server/Server.hpp"
#include <vector>
#include "EventHandlers/HttpEventHandler.hpp"
#include <signal.h>
#include "ConfigHandler/Headers/Preprocessor.hpp"

void HandleSigPip(int signal)
{

    std::string msg = "SigPip " + SSTR(signal) + "\n";
    write(1, msg.c_str(), msg.size());
}

int main(int ac, char **av)
{
    // Preprocessor pre
    std::stringstream content;
    if (ac > 1)
    {
        std::ifstream inputFile(av[1]);
        if (!inputFile)
            throw std::runtime_error("Invalide File");
        content << inputFile.rdbuf();
        std::string str = content.str();
        Preprocessor processor(str);
    }
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