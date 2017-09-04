#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_PORT 9999
#define MAX_FD_SETSIZE 1001
#define MAX_LISTEN 1000
#define MAX_LEN 4096

int main(int argc, char *argv[]) {
    int listenfd, connfd;
    int byte_read;
    short int port = DEFAULT_PORT;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char buffer[MAX_LEN];
    fd_set active_fd_set, read_fd_set;

    if (argc > 2) {
        printf("usage: ./server port"), exit(EXIT_FAILURE);
    } else if (argc == 2) {
        if ((port = strtol(argv[1], NULL, 0)) == 0) {
            perror("invalid port number"), exit(EXIT_FAILURE);
        }
    }

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error opening socket"), exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if ((bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
        perror("Error binding"), exit(EXIT_FAILURE);
    }

    if (listen(listenfd, MAX_LISTEN) < 0) {
        perror("Error listening"), exit(EXIT_FAILURE);
    }

    FD_ZERO(&active_fd_set);
    FD_SET(listenfd, &active_fd_set);

    while (1) {
        read_fd_set = active_fd_set;
        if (select(MAX_FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
            perror("Error selecting"), exit(EXIT_FAILURE);
        } 
        for (int i = 0; i < MAX_FD_SETSIZE; ++i) {
            if (FD_ISSET(i, &read_fd_set)) {
                if (i == listenfd) {
                    clilen = sizeof(cliaddr);
                    if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
                        perror("Error accepting"), exit(EXIT_FAILURE);
                    }

                    FD_SET(connfd, &active_fd_set);
                } else {
                    byte_read = read(i, buffer, MAX_LEN); 
                    if (byte_read == -1) {
                        perror("Error reading from client"), exit(EXIT_FAILURE);
                    } else if (byte_read == 0) {
                        if (close(i) < 0) {
                            perror("Error closing connfd"), exit(EXIT_FAILURE);
                        }

                        FD_CLR(i, &active_fd_set);
                    } else {
                        if (write(i, buffer, byte_read) == -1) {
                            perror("Error writing to client"), exit(EXIT_FAILURE);
                        }
                    }

                    memset(buffer, 0, MAX_LEN);
                }
            }
        }
    }
    
    return 0;
}
