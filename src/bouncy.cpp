#include <stdio.h>

#define BOUNCY_DEBUG 1

#include "json.h"
#include "plat.h"

struct main_config
{
    size_t maxReadSize;
};

int main(int argc, char **argv)
{
    proc_info proc_info = platGetProcInfo(argv[0]);
    // main_config config = {MB(4)};
    main_config config = {1};

    // printf("cwd: %s\n", proc_info.cwd);
    // printf("procName: %s\n", proc_info.procName);

    {
        char test_json_path[PATH_MAX];
        strcpy(test_json_path, proc_info.cwd);
        strcat(test_json_path, "/config.json");
        text_file test_json = platBeginTextFileRead(test_json_path);
        if (test_json.size < config.maxReadSize)
        {
            text_file_buf config_buf = platReadEntireTextFile(&test_json);
            json_parse_result jsonParseResult = jsonParse(config_buf.data, config_buf.textLen);
            platFreeFileBuf(&config_buf);
        }
        else
        {
            fprintf(stdout, "config.json is larger than maxReadSize(%lu)", config.maxReadSize);
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

#if 0

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
#endif
