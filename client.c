#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define DEFAULT_PORT 8888
#define DEFAULT_HOST "127.0.0.1"
#define MAX_LEN 4096

int main(int argc, char *argv[]) {
    char buffer[MAX_LEN];
    int sockfd;
    int bytes_received;
    short int port = DEFAULT_PORT;
    char host[256] = DEFAULT_HOST;
    struct sockaddr_in servaddr;

    if (argc > 3) {
        printf("usage: ./client hostname port"), exit(EXIT_FAILURE);
    } else if (argc == 3) {
        if ((port = strtol(argv[2], NULL, 0)) == 0) {
            perror("invalid port number"), exit(EXIT_FAILURE);
        }

        strcpy(host, argv[1]);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error opening socket"), exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, host, &servaddr.sin_addr);
 
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Error connecting"), exit(EXIT_FAILURE);
    }

    while (fgets(buffer, MAX_LEN, stdin) != NULL) {
        if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
            perror("Error sending buffer"), exit(EXIT_FAILURE);
        }

        if ((bytes_received = recv(sockfd, buffer, MAX_LEN, 0)) < 0) {
            perror("Error receiving message"), exit(EXIT_FAILURE);
        }

        printf("%s", buffer);
    }   
    
    close(sockfd);
    return EXIT_SUCCESS;
}
