#include "Server/Server.hpp"
#include "SSL/OpenSSLLoader.hpp"
#include <vector>
#include <signal.h>
#include "ConfigHandler/Headers/ConfigHandler.hpp"
#include "Core/Include/Core.hpp"

void HandleSigPip(int signal)
{
    std::string msg = "SigPip " + SSTR(signal) + "\n";
    write(1, msg.c_str(), msg.size());
}

void InitLibSSL(std::vector<ServerClass> ServersConf)
{
    for (size_t i = 0; i < ServersConf.size(); i++)
    {
        if (!ServersConf.at(i).IsSSlOn())
            continue;
        OpenSSLLoader::LoadOpenSSL();
        DEBUGOUT(1, COLORED(OpenSSLLoader::my_OpenSSL_version(SSLEAY_VERSION), Blue));
        return;
    }
}

int main(int ac, char **av)
{
    std::vector<std::string> taken_ports;
    std::vector<ServerClass> ServersConf;
    Server server;

    try
    {
        if (ac != 2)
            throw std::runtime_error("Please Provide a Conf file");
        ConfigHandler::SetFile(av[1]);
        signal(SIGPIPE, HandleSigPip);
        ServersConf = ConfigHandler::GetHttp().GetServers();
        InitLibSSL(ServersConf);
        for (size_t i = 0; i < ServersConf.size(); i++)
        {
            if (Containes(taken_ports, ServersConf.at(i).GetPort()))
                continue;
            server.CreatSocket(ServersConf.at(i));
            taken_ports.push_back(ServersConf.at(i).GetPort());
        }
        server.Run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return (1);
    }
    return (0);
}