#include "OpenSSLLoader.hpp"
#include <openssl/ssl.h>

void *OpenSSLLoader::opensslLibrary = NULL;

OPENSSL_init_ssl_ptr OpenSSLLoader::my_OPENSSL_init_ssl;
SSL_new_ptr OpenSSLLoader::my_SSL_new;
SSL_accept_ptr OpenSSLLoader::my_SSL_accept;
SSL_read_ptr OpenSSLLoader::my_SSL_read;
SSL_write_ptr OpenSSLLoader::my_SSL_write;
SSL_shutdown_ptr OpenSSLLoader::my_SSL_shutdown;
OpenSSL_version_ptr OpenSSLLoader::my_OpenSSL_version;
SSL_free_ptr OpenSSLLoader::my_SSL_free;
SSL_set_fd_ptr OpenSSLLoader::my_SSL_set_fd;

SSL_CTX_new_ptr OpenSSLLoader::my_SSL_CTX_new;
TLS_server_method_ptr OpenSSLLoader::my_TLS_server_method;
SSL_CTX_use_certificate_file_ptr OpenSSLLoader::my_SSL_CTX_use_certificate_file;
SSL_CTX_use_PrivateKey_file_ptr OpenSSLLoader::my_SSL_CTX_use_PrivateKey_file;

OpenSSLLoader::OpenSSLLoader()
{
}

void *OpenSSLLoader::LoadOpenSSL()
{
    (opensslLibrary = dlopen("libssl.so", RTLD_NOW | RTLD_GLOBAL));
    if (!opensslLibrary)
        throw std::runtime_error("Error dlopen OpenSSL: " + std::string(dlerror()));

    my_OPENSSL_init_ssl = get_symbol<OPENSSL_init_ssl_ptr>(opensslLibrary, "OPENSSL_init_ssl");

    my_SSL_new = get_symbol<SSL_new_ptr>(opensslLibrary, "SSL_new");
    my_SSL_accept = get_symbol<SSL_accept_ptr>(opensslLibrary, "SSL_accept");
    my_SSL_read = get_symbol<SSL_read_ptr>(opensslLibrary, "SSL_read");
    my_SSL_write = get_symbol<SSL_write_ptr>(opensslLibrary, "SSL_write");
    my_SSL_shutdown = get_symbol<SSL_shutdown_ptr>(opensslLibrary, "SSL_shutdown");

    my_SSL_CTX_new = get_symbol<SSL_CTX_new_ptr>(opensslLibrary, "SSL_CTX_new");
    my_TLS_server_method = get_symbol<TLS_server_method_ptr>(opensslLibrary, "TLS_server_method");
    my_SSL_CTX_use_certificate_file = get_symbol<SSL_CTX_use_certificate_file_ptr>(opensslLibrary, "SSL_CTX_use_certificate_file");
    my_SSL_CTX_use_PrivateKey_file = get_symbol<SSL_CTX_use_PrivateKey_file_ptr>(opensslLibrary, "SSL_CTX_use_PrivateKey_file");

    my_OpenSSL_version = get_symbol<OpenSSL_version_ptr>(opensslLibrary, "OpenSSL_version");
    my_SSL_free = get_symbol<SSL_free_ptr>(opensslLibrary, "SSL_free");
    my_SSL_set_fd = get_symbol<SSL_set_fd_ptr>(opensslLibrary, "SSL_set_fd");
    // Initialize OpenSSL
    my_OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);
    return (opensslLibrary);
}

OpenSSLLoader::~OpenSSLLoader()
{
}