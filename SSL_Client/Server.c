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

#define READSIZE 4096

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

void multiplexing(int socket, SSL *ssl);

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

    int resure_flag = 1;

    if ((socket_fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) < 0)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &resure_flag, sizeof(resure_flag)) < 0)
    {
        perror("setsockopt() failed");
        exit(EXIT_FAILURE);
    }

    if (bind(socket_fd, addr->ai_addr, addr->ai_addrlen) < 0)
    {
        perror("connect() failed");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(addr);
    if (listen(socket_fd, 100) < 0)
    {
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }
    return (socket_fd);
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
    char read[4096];
    int socket_fd;
    SSL_CTX *ctx;
    X509 *cert;
    char *tmp;
    long vp;

    printf("%s\n", OpenSSL_version(SSLEAY_VERSION));
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx)
    {
        fprintf(stderr, "SSL_CTX_new() failed.\n");
        return 1;
    }

    if (!SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) ||
        !SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM))
        return fprintf(stderr, "SSL_connect() failed.\n"), EXIT_FAILURE;

    socket_fd = InitSocket("127.0.0.1", "8080");
    printf("socket_fd %d\n", socket_fd);

    while (1)
    {
        SSL *ssl;
        int client_fd = accept(socket_fd, 0, 0);
        if (client_fd < 0)
        {
            fprintf(stderr, "accept() failed.\n");
            return EXIT_FAILURE;
        }
        ssl = SSL_new(ctx);
        if (!ssl)
        {
            fprintf(stderr, "SSL_new() failed.\n");
            return EXIT_FAILURE;
        }
        SSL_set_fd(ssl, client_fd);
        if (SSL_accept(ssl) < 0)
        {
            fprintf(stderr, "SSL_accept() failed.\n");

            SSL_shutdown(ssl);
            close(client_fd);
            SSL_free(ssl);
            continue;
        }
        printf("SSL connection using %s\n", SSL_get_cipher(ssl));

        printf("reading from clinet\n");
        char Rsv[READSIZE];
        int bytes = SSL_read(ssl, Rsv, READSIZE);
        printf("RSV %d bytes %.*s\n", bytes, bytes, Rsv);

        printf("Sending response...\n");
        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Connection: close\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "Local time is: ";
        int bytes_sent = SSL_write(ssl, response, strlen(response));
        printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response));

        time_t timer;
        time(&timer);
        char *time_msg = ctime(&timer);
        bytes_sent = SSL_write(ssl, time_msg, strlen(time_msg));
        printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));

        printf("Closing connection...\n");
        SSL_shutdown(ssl);
        close(client_fd);
        SSL_free(ssl);
    }
    SSL_CTX_free(ctx);
    close(socket_fd);
}
