#include <openssl/ssl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

enum
{
    true = 1,
    false = 0
};

#define READSIZE 4096


int LoadOpenSSLLib()
{
    // void *ssl_lib = dlopen("libssl.so", RTLD_LAZY);
    // if (!ssl_lib)
    // {
    //     fprintf(stderr, "dlopen() failled %s\n", dlerror());
    //     exit(EXIT_FAILURE);
    // }
}

void multiplexing(int socket, SSL *ssl, char *Host);

int InitSocket(char *host, char *port)
{
    struct addrinfo hint;
    struct addrinfo *addr;
    int socket_fd;

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;

    if (getaddrinfo(host, port, &hint, &addr) < 0)
    {
        perror("getaddrinfo() failed");
        exit(EXIT_FAILURE);
    }
    if ((socket_fd = socket(addr->ai_family, addr->ai_socktype, 0)) < 0)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }
    if (connect(socket_fd, addr->ai_addr, addr->ai_addrlen) < 0)
    {
        perror("connect() failed");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(addr);
    return (socket_fd);
}

char *ConcatStrings(char *str1, char *str2)
{
    if (!str1)
        return (strdup(str2));
    size_t len1 = 0, len2 = 0;
    while (str1[len1] != '\0')
        len1++;
    while (str2 && str2[len2] != '\0')
        len2++;
    char *output = malloc(len1 + len2);
    sprintf((char *)output, "%s%s", str1, str2);
    return output;
}

char *read_socket(SSL *ssl)
{
    char *Buffer = malloc(READSIZE);

    int n_bytes = SSL_read(ssl, Buffer, READSIZE);
    if (n_bytes < 1)
    {
        printf("connection closed by peer\n");
        return NULL;
    }
    Buffer[n_bytes] = 0;
    return (Buffer);
}

int main()
{
    int bytes_received;
    int max_socket;
    char read[4096];
    int socket_fd;
    SSL_CTX *ctx;
    SSL *ssl;
    X509 *cert;
    char *tmp;
    long vp;

    // printf("%s\n", OpenSSL_version(SSLEAY_VERSION));
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new_ptr(TLS_client_method());
    ssl = SSL_new_ptr(ctx);
    socket_fd = InitSocket("chat.openai.com", "443");

    if (!SSL_set_tlsext_host_name(ssl, "chat.openai.com"))
        return fprintf(stderr, "SSL_connect() failed.\n"), EXIT_FAILURE;

    SSL_set_fd(ssl, socket_fd);

    if (SSL_connect(ssl) == -1)
        return fprintf(stderr, "SSL_connect() failed.\n"), EXIT_FAILURE;

    cert = SSL_get_peer_certificate(ssl);
    if (!cert)
        return fprintf(stderr, "SSL_get_peer_certificate() failed.\n"), EXIT_FAILURE;
    if (tmp = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0))
    {
        printf("subject name %s\n", tmp);
        OPENSSL_free(tmp);
    }
    if (tmp = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0))
    {
        printf("issuer name %s\n", tmp);
        OPENSSL_free(tmp);
    }
    X509_free(cert);

    vp = SSL_get_verify_result(ssl);
    if (vp == X509_V_OK)
        printf("--> cert was verified \n");
    else
        printf("--> cert cound not be verified (%ld) \n", vp);

    printf("SSL State after connect: %s\n", SSL_state_string(ssl));
    printf("cipher that being used : `%s`\n", SSL_get_cipher(ssl));

    multiplexing(socket_fd, ssl, "chat.openai.com");

    SSL_shutdown(ssl);
    close(socket_fd);
    SSL_free(ssl);
}

fd_set InitSet(int socket)
{
    fd_set socket_set;
    FD_ZERO(&socket_set);
    FD_SET(socket, &socket_set);
    return (socket_set);
}

void multiplexing(int socket, SSL *ssl, char *Host)
{
    char *Data = NULL;
    char *end_addr;
    int Sent = false;
    for (;;)
    {
        fd_set reads = InitSet(socket);
        fd_set writes = InitSet(socket);
        struct timeval timeout;

        timeout.tv_sec = 0;
        timeout.tv_usec = 1000000;

        if (select(socket + 1, &reads, &writes, NULL, &timeout) < 0)
        {
            fprintf(stderr, "select() failed (%d)\n", errno);
            exit(EXIT_FAILURE);
        }
        if (FD_ISSET(socket, &reads))
        {
            char *Buffer = read_socket(ssl);
            if (!Buffer)
                break;
            Data = ConcatStrings(Data, Buffer);
            if ((end_addr = strstr(Data, "\r\n\r\n")) != NULL)
            {
                printf("\n\n");
                for (int i = 0; (Data + i) != end_addr; i++)
                    printf("%c", Data[i]);
                printf("\n\n");

                free(Data);
                Data = NULL;
                end_addr = 0;
            }
        }
        if (!Sent && FD_ISSET(socket, &writes))
        {

            char Request[READSIZE];
            int bytes;
            sprintf(Request, "GET / HTTP/1.1\r\n"
                             "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
                             "Host: %s\r\n"
                             "Accept-Language: en-us\r\n"
                             "Accept-Encoding: chunk\r\n"
                             "Connection: close\r\n"
                             "\r\n",
                    Host);
            if (!Sent && (bytes = SSL_write(ssl, Request, strlen(Request))) < 0)
            {
                fprintf(stderr, "SSL_connect() failed.\n");
                return;
            }
            printf("sent %d\nRequest:\n%s\n\n", bytes, Request);
            Sent = true;
        }
    }
}
