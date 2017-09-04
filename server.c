#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_PORT 9999
#define MAX_LEN 4096
#define LOG_FILE_NAME "log.txt"

FILE *log_file;

char *get_current_time(char *buffer, int size) {
    time_t timer;
    struct tm* time_info;
    time(&timer);
    time_info = localtime(&timer);
  
    strftime(buffer, size, "%H:%M:%S %D", time_info);
    return buffer;
}

void log_msg(const char *message) {
    char buffer[26];
    fprintf(log_file, "%s %s: %s\n", get_current_time(buffer, 26), strerror(errno), message);
    fflush(log_file);
}

void log_client(const char *user_info) {
    char buffer[26];
    fprintf(log_file, "%s %s", get_current_time(buffer, 26), user_info);
    fflush(log_file);
}

void close_log_file() {
    if (fclose(log_file) == EOF) {
        log_msg("Error closing log file");
    }
}

void create_log_file() {
    if ((log_file = fopen(LOG_FILE_NAME, "w")) == NULL) {
        perror("Error creating log file"), exit(EXIT_FAILURE);
    }
}

void shutdown_server() {
    close_log_file(log_file);
    log_msg("Shutdown server");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int listenfd, connfd;
    int byte_read;
    short int port = DEFAULT_PORT;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char buffer[MAX_LEN];
    fd_set active_fd_set, read_fd_set;

    create_log_file();
    log_msg("start server");
    
    if (argc > 2) {
        errno = EXIT_FAILURE;
        log_msg("usage: ./server port"), shutdown_server();
    } else if (argc == 2) {
        if ((port = strtol(argv[1], NULL, 0)) == 0) {
            errno = EXIT_FAILURE;
            log_msg("invalid port number"), shutdown_server();
        }
    }

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log_msg("Error opening socket"), shutdown_server();
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if ((bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
        log_msg("Error binding"), shutdown_server();
    }

    if (listen(listenfd, FD_SETSIZE) < 0) {
        log_msg("Error listening"), shutdown_server();
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
                    
                    log_client(inet_ntoa(cliaddr.sin_addr));     
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
