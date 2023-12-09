#include <openssl/ssl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

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
    size_t len1 = 0, len2 = 0;
    while (str1 && str1[len1] != '\0')
        len1++;
    while (str2 && str2[len2] != '\0')
        len2++;
    char *output = malloc(len1 + len2);
    sprintf((char *)output, "%s%s", str1, str2);
    return output;
}

void read_socket(SSL *ssl)
{
    char Buffer[1024];
    char *Data = NULL;
    while (1)
    {
        int n_bytes = SSL_read(ssl, Buffer, sizeof(Buffer));
        if (n_bytes < 0)
        {
            fprintf(stderr, "read() failed");
            exit(1);
        }
        printf(": %s\n", Buffer);

        Data = ConcatStrings(Data, Buffer);
        if (strstr(Data, "\r\n\r\r\n"))
        {
            printf("headers :\n%s", Data);
            break;
        }
    }
}

int main()
{
    int bytes_received;
    char read[4096];
    int Socket_fd;
    SSL_CTX *ctx;
    SSL *ssl;

    printf("%s\n", OpenSSL_version(SSLEAY_VERSION));
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(TLS_client_method());
    ssl = SSL_new(ctx);
    Socket_fd = InitSocket("chat.openai.com", "443");
    if (!SSL_set_tlsext_host_name(ssl, "chat.openai.com"))
    {
        fprintf(stderr, "SSL_connect() failed.\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    printf("Socket_fd : %d\n", Socket_fd);
    SSL_set_fd(ssl, Socket_fd);
    if (SSL_connect(ssl) == -1)
    {
        fprintf(stderr, "SSL_connect() failed.\n");
        ERR_print_errors_fp(stderr);
        return 0;
    }
    // int ssl_state = SSL_get_state(ssl);
    printf("SSL State after connect: %s\n", SSL_state_string(ssl));
    printf("cipher that being used : `%s`\n", SSL_get_cipher(ssl));
    
    bytes_received = SSL_read(ssl, read, 4096);
    printf("Received: %.*s\n", bytes_received, read);
    
    SSL_shutdown(ssl);
    close(Socket_fd);
    SSL_free(ssl);
}
