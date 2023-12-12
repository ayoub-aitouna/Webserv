#include "VirtualServer.hpp"

VirtualServer::VirtualServer(int SocketFd, bool SSlOn, std::string cert, std::string Key)
{
    this->ctx = NULL;
    this->SSlOn = SSlOn;
    this->socketFd = SocketFd;
    if (SSlOn)
    {
        ctx = OpenSSLLoader::my_SSL_CTX_new(OpenSSLLoader::my_TLS_server_method());
        if (!OpenSSLLoader::my_SSL_CTX_use_certificate_file(ctx, cert.c_str(), SSL_FILETYPE_PEM) ||
            !OpenSSLLoader::my_SSL_CTX_use_PrivateKey_file(ctx, Key.c_str(), SSL_FILETYPE_PEM))
            std::runtime_error("SSL_CTX_use_*() failed.");
    }
}

void *VirtualServer::getSLL_CTX()
{
    return (this->ctx);
}

bool VirtualServer::IsSSlOn()
{

    return (this->SSlOn);
}

int VirtualServer::GetSocketFd()
{
    return (this->socketFd);
}

VirtualServer::~VirtualServer()
{
}