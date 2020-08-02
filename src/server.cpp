#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <curl/curl.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>

int start_server(const char *port)
{
    int listenfd;

    addrinfo hints;
    addrinfo *result_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int gai_result = getaddrinfo(NULL, port, &hints, &result_info);
    if(gai_result != 0)
    {
#if BOUNCY_DEBUG
        perror(gai_strerror(gai_result));
#else
        perror("getaddrinfo() error");
#endif
        exit(1);
    }

    addrinfo *current_info;
    for(current_info = result_info;
        current_info != NULL;
        current_info = current_info->ai_next)
    {
        int option = 1;
        listenfd = socket(current_info->ai_family, current_info->ai_socktype, 0);
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        if(listenfd == -1)
        {
            continue;
        }
        else
        {
            int bind_result = bind(listenfd, current_info->ai_addr, current_info->ai_addrlen);
            if(bind_result == 0)
            {
                break;
            }
        }
    }

    if(current_info == NULL)
    {
        perror("socket() or bind()");
        exit(1);
    }

    freeaddrinfo(result_info);

    int max_backlog = 3;
    int listen_result = listen(listenfd, max_backlog);
    if (listen_result != 0)
    {
        perror("listen() error");
        exit(1);
    }

    return listenfd;
}

int start_epoll(int listenfd)
{
    int epoll_fd = epoll_create(1);
    if(epoll_fd == -1)
    {
        perror("epoll_create() error");
        exit(1);
    }

    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = listenfd;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listenfd, &event) == -1)
    {
        perror("epoll_ctl() error");
        close(epoll_fd);
        exit(1);
    }

    return epoll_fd;
}

// void fetch_example(void) {
//     CURL *curl = curl_easy_init();
//     if(curl) {
//         CURLcode res;
//         curl_easy_setopt(curl, CURLOPT_URL, "http://example.com");
//         res = curl_easy_perform(curl);
//         curl_easy_cleanup(curl);
//     }
// }

typedef struct {
    char *name;
    char *value;
} header_t;

char *request_header(const char* name, header_t *reqhdr)
{
    header_t *h = reqhdr;
    while(h && h->name) {
        if (strcmp(h->name, name) == 0)
        {
            return h->value;
        }
        ++h;
    }
    return NULL;
}

void handle_request(int client_fd)
{
    char *buf;
    size_t buf_size = 65535;

    buf = (char *)malloc(buf_size);
    int rcvd = recv(client_fd, buf, buf_size, 0);
    if(rcvd < 0)
    {
        fprintf(stderr, "recv() error\n");
    }
    else if(rcvd == 0)
    {
        fprintf(stderr, "Client disconnected unexpectedly.\n");
    }
    else
    {
        char *method;
        char *uri;
        char *prot;

        buf[rcvd] = '\0';

        method = strtok(buf, " \t\r\n");
        uri = strtok(NULL, " \t");
        prot = strtok(NULL, " \t\r\n");

        fprintf(stderr, "\x1b[32m + [%s] %s\x1b[0m\n", method, uri);

        header_t reqhdr[17] = {};
        header_t *h = reqhdr;
        // TODO: bulletproof this
        while(h < reqhdr+16)
        {
            char *key;
            char *value;
            key = strtok(NULL, "\r\n: \t");
            if (!key)
            {
                break;
            }
            value = strtok(NULL, "\r\n");
            uint addtl_length = 0;
            while(*value && *value == ' ')
            {
                ++value;
                ++addtl_length;
            }
            h->name = key;
            h->value = value;
            ++h;
            fprintf(stderr, "[H] %s: %s\n", key, value);
            char *temp = value + 1 + addtl_length + strlen(value);
            if (temp[0] == '\r' && temp[1] == '\n')
            {
                break;
            }
        }

        const char *http_1_1 = "HTTP/1.1";
        write(client_fd, http_1_1, strlen(http_1_1));
        const char *http_end = "\r\n\r\n";

        if(strncmp(method, "GET", 3) == 0)
        {
            if(strlen(uri) == 1 && strncmp(uri, "/", 1) == 0)
            {
                write(client_fd, " 200 OK", 7);
                write(client_fd, http_end, strlen(http_end));

                char message[255];
                sprintf(message,
                    "Hello! You are using %s", request_header("User-Agent", reqhdr));
                write(client_fd, message, strlen(message));
            }
            else
            {
                write(client_fd, " 404 Not Found", 14);
                write(client_fd, http_end, strlen(http_end));
            }
        }
        else
        {
            write(client_fd, " 405 Method Not Allowed", 23);
            write(client_fd, http_end, strlen(http_end));
        }

        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);

        free(buf);
    }
}
