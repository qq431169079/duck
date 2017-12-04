#define _GNU_SOURCE

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "http.h"
#include "log.h"

#define DEFAULT_PORT 9999

int parse_command_argument(int argc, char *argv[], short int *port) {
    if (argc > 2) {
        errno = EXIT_FAILURE;
        log_msg("usage: ./server port");
        return -1;
    } else if (argc == 2) {
        if ((*port = strtol(argv[1], NULL, 0)) == 0) {
            errno = EXIT_FAILURE;
            log_msg("invalid port number");
            return -1;
        }
    }
    return 0;
}

void shutdown_server(int signum) {
    log_msg("Shutdown server");
    close_log_file();
    exit(EXIT_FAILURE);
}

int setup_listenfd(const short int port) {
    int listenfd = 0;
    struct sockaddr_in servaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log_msg("Error opening socket");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if ((bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
        log_msg("Error binding");
        return -1;
    }
    if (listen(listenfd, FD_SETSIZE) < 0) {
        log_msg("Error listening");
        return -1;
    }

    return listenfd;
}

int main(int argc, char *argv[]) {
    int listenfd, connfd;
    int current_process_id;
    short int port = DEFAULT_PORT;
    struct sockaddr_in cliaddr;
    socklen_t clilen;
    char buffer[MAX_LEN];
    fd_set active_fd_set, read_fd_set;
    

    http_connection *con[FD_SETSIZE];
    init_http(con);
   
    init_log((const http_connection **)con);

    current_process_id = getpid();
    signal(SIGTERM, shutdown_server);

    if (parse_command_argument(argc, argv, &port) == -1) {
        kill(current_process_id, SIGTERM);
    }

    if ((listenfd = setup_listenfd(port)) == -1) {
        kill(current_process_id, SIGTERM);
    }

    FD_ZERO(&active_fd_set);
    FD_SET(listenfd, &active_fd_set);
    log_msg("Start server");

    while (1) {
        read_fd_set = active_fd_set;
        if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
            log_msg("Error selecting");
        } 
        for (int i = 0; i < FD_SETSIZE; ++i) {
            if (FD_ISSET(i, &read_fd_set)) {
                if (i == listenfd) {
                    clilen = sizeof(cliaddr);
                    if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
                        log_msg("Error accepting");
                    } 
                    
                    add_connection(connfd, &cliaddr);
                    FD_SET(connfd, &active_fd_set);
                } else {
                    int status = connection_handler(con[i]);
                    if (status == 0) {
                        if (close(i) < 0) {
                            log_msg("Error closing connfd");
                        }

                        FD_CLR(i, &active_fd_set);
                        remove_connection(i);
                    } else if (status == -1) {
                        // TODO: what to do if error occurs? specify types of error. 1. has no url
                        if (close(i) < 0) {
                            log_msg("Error closing connfd");
                        }

                        FD_CLR(i, &active_fd_set);
                        remove_connection(i);
                    }

                    memset(buffer, 0, MAX_LEN);
                }
            }
        }
    }
}
