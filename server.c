#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "log.h"
#include "client.h"

#define DEFAULT_PORT 9999


void shutdown_server(int signum) {
    log_msg("Shutdown server");
    close_log_file(log_file);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int listenfd, connfd;
    int byte_read;
    int current_process_id;
    short int port = DEFAULT_PORT;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char buffer[MAX_LEN];
    fd_set active_fd_set, read_fd_set;

    create_log_file();
    log_msg("Start server");

    current_process_id = getpid();
    signal(SIGTERM, shutdown_server);

    if (argc > 2) {
        errno = EXIT_FAILURE;
        log_msg("usage: ./server port"), kill(current_process_id, SIGTERM);
    } else if (argc == 2) {
        if ((port = strtol(argv[1], NULL, 0)) == 0) {
            errno = EXIT_FAILURE;
            log_msg("invalid port number"), kill(current_process_id, SIGTERM);
        }
    }

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log_msg("Error opening socket"), kill(current_process_id, SIGTERM);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if ((bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
        log_msg("Error binding"), kill(current_process_id, SIGTERM);
    }

    if (listen(listenfd, FD_SETSIZE) < 0) {
        log_msg("Error listening"), kill(current_process_id, SIGTERM);
    }

    FD_ZERO(&active_fd_set);
    FD_SET(listenfd, &active_fd_set);

    while (1) {
        read_fd_set = active_fd_set;
        if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
            log_msg("Error selecting");
            continue;
        } 
        for (int i = 0; i < FD_SETSIZE; ++i) {
            if (FD_ISSET(i, &read_fd_set)) {
                if (i == listenfd) {
                    clilen = sizeof(cliaddr);
                    if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
                        log_msg("Error accepting");
                        continue;
                    } 
                    
                    log_client(inet_ntoa(cliaddr.sin_addr), "connected");     
                    FD_SET(connfd, &active_fd_set);
                } else {
                    byte_read = read(i, buffer, MAX_LEN); 
                    if (byte_read == -1) {
                        log_msg("Error reading from client");
                        continue;
                    } else if (byte_read == 0) {
                        if (close(i) < 0) {
                            log_msg("Error closing connfd");
                            continue;
                        }

                        FD_CLR(i, &active_fd_set);
                    } else {
                        if (write(i, buffer, byte_read) == -1) {
                            log_msg("Error writing to client");
                            continue;
                        }
                    }

                    memset(buffer, 0, MAX_LEN);
                }
            }
        }
    }
}
