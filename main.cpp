#include "Server/Server.hpp"
#include <vector>
#include <signal.h>
#include "ConfigHandler/Headers/ConfigHandler.hpp"
#include "Proccesers/Include/Proccesers.hpp"

void HandleSigPip(int signal)
{
    std::string msg = "SigPip " + SSTR(signal) + "\n";
    write(1, msg.c_str(), msg.size());
}


/**
 TODO example-dir didnt ditect index.html when there is 2 servers
    post on cgi error find out why 

*/
int main(int ac, char **av)
{
    std::vector<std::string> taken_ports;
    if (ac != 2)
    {
        std::cout << "Please Provide a Conf file" << std::endl;
        exit(1);
    }
    ConfigHandler::SetFile(av[1]);
    signal(SIGPIPE, HandleSigPip);
    std::vector<ServerClass> ServersConf = ConfigHandler::GetHttp().GetServers();
    Server server;
    try
    {
        for (size_t i = 0; i < ServersConf.size(); i++)
        {
            if (Containes(taken_ports, ServersConf.at(i).GetPort()))
                continue;
            server.CreatSocket(ServersConf.at(i).GetHostName(), ServersConf.at(i).GetPort());
            taken_ports.push_back(ServersConf.at(i).GetPort());
        }
        server.Run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}