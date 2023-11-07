#include "Server/Server.hpp"

int main()
{
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