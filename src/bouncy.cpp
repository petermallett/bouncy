#include <stdio.h>

#include "plat.h"

#include "plat.cpp"
#include "server.cpp"

#define BOUNCY_DEBUG 1

#define MAX_EVENTS 3
#define POLL_INTERVAL 3000

void run_server(int epoll_fd)
{
    bool running = true;
    int event_count = 0;
    epoll_event events[MAX_EVENTS];

    sockaddr_in client_addr;
    socklen_t addrlen;
    int client_fd;

    while(running)
    {
        event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, POLL_INTERVAL);
        fprintf(stderr, "%d events in %3.2fs\n", event_count, POLL_INTERVAL/1000.0);
        for (int i = 0; i < event_count; ++i)
        {
            addrlen = sizeof(client_addr);
            client_fd = accept(events[i].data.fd, (struct sockaddr *)&client_addr, &addrlen);
            if(client_fd < 0)
            {
                perror("accept() error");
                running = false;
            }
            else
            {
                handle_request(client_fd);
            }
        }

        // do other work here
    }
}

enum JSON_state {
    JSON_STRING,
    JSON_NUMBER,
    JSON_NULL,
    JSON_TRUE,
    JSON_FALSE,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_EOF,

    JSON_ANY,
};
struct parse_result {
    char message[255];
    char error[255];
    int status;
};
parse_result
parseJSON(text_file_buf *buf_info)
{
    parse_result result = {};
    JSON_state state = JSON_ANY;
    char c;

    if(buf_info->fileInfo->size == 0)
    {
        c = EOF;
        result.status = -1;
        strcpy(result.message, "Error: Parse error on line 1:");
        // sprintf(result.error, "Expected one of: %s, got %s",
        //         parseJSON_expecting(JSON_ANY), parseJSON_saw(JSON_EOF));
    }

    return result;
}

int main(int argc, char **argv)
{
    proc_info proc_info = platGetProcInfo(argv[0]);

    // printf("cwd: %s\n", proc_info.cwd);
    // printf("procName: %s\n", proc_info.procName);

    {
        char test_json_path[PATH_MAX];
        strcpy(test_json_path, proc_info.cwd);
        strcat(test_json_path, "/test.json");
        text_file test_json = platBeginTextFileRead(test_json_path);
        if (test_json.size < MB(4))
        {
            text_file_buf test_json_buf = platReadEntireTextFile(&test_json);
            platFreeFileBuf(&test_json_buf);
        }
        platEndTextFileRead(&test_json);
    }

    // if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
    //     perror("curl global init failed");
    //     exit(1);
    // }

    // int listenfd = start_server("8080");
    // int epoll_fd = start_epoll(listenfd);
    // run_server(epoll_fd);

    // close(epoll_fd);

    return 0;
}
