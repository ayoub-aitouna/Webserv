#ifndef OPENSSLLOADER_HPP
#define OPENSSLLOADER_HPP

#include <openssl/ssl.h>
#include <string>
#include <dlfcn.h>
#include <stdexcept>
#include <iostream>
#include <unistd.h>

// OpenSSL function pointers
typedef int (*OPENSSL_init_ssl_ptr)(uint64_t, const OPENSSL_INIT_SETTINGS*);

typedef SSL *(*SSL_new_ptr)(void *);
typedef int (*SSL_accept_ptr)(void *);
typedef int (*SSL_read_ptr)(void *, void *, int);
typedef int (*SSL_write_ptr)(void *, const void *, int);
typedef int (*SSL_shutdown_ptr)(void *);
typedef char (*OpenSSL_version_ptr)(int);
typedef void (*SSL_free_ptr)(SSL *);
typedef int (*SSL_set_fd_ptr)(SSL *, int);

typedef SSL_CTX *(*SSL_CTX_new_ptr)(const SSL_METHOD *);
typedef int (*SSL_CTX_use_certificate_file_ptr)(void *, const char *, int);
typedef int (*SSL_CTX_use_PrivateKey_file_ptr)(void *, const char *, int);

typedef SSL_METHOD *(*TLS_server_method_ptr)(void);

class OpenSSLLoader
{
private:
    OpenSSLLoader();
    ~OpenSSLLoader();

public:
    static void *LoadOpenSSL();

private:
    static void *opensslLibrary;
    // Function pointers
public:
    static OPENSSL_init_ssl_ptr my_OPENSSL_init_ssl;


    static SSL_new_ptr my_SSL_new;
    static SSL_accept_ptr my_SSL_accept;
    static SSL_read_ptr my_SSL_read;
    static SSL_write_ptr my_SSL_write;
    static SSL_shutdown_ptr my_SSL_shutdown;
    static OpenSSL_version_ptr my_OpenSSL_version;
    static SSL_free_ptr my_SSL_free;
    static SSL_set_fd_ptr my_SSL_set_fd;

    static SSL_CTX_new_ptr my_SSL_CTX_new;
    static TLS_server_method_ptr my_TLS_server_method;
    static SSL_CTX_use_certificate_file_ptr my_SSL_CTX_use_certificate_file;
    static SSL_CTX_use_PrivateKey_file_ptr my_SSL_CTX_use_PrivateKey_file;
};

template <typename T>
T get_symbol(void *handler, const char *symbol_name)
{
    T symbol_ptr = (T)dlsym(handler, symbol_name);
    if (!symbol_ptr)
    {
        std::cerr << "Error get_symbol OpenSSL: " << dlerror() << std::endl;
        dlclose(handler);
        exit(1);
    }
    return (symbol_ptr);
}

#endif