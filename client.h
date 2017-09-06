#ifndef CLIENT_H_
#define CLIENT_H_

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#define MAX_LEN 4096
#define MAX_IP 128

struct Client {
    int connfd;
    char ip[MAX_IP];
    char write_buffer[MAX_LEN];
};

void set_ip(struct Client *client, struct sockaddr_in *cliaddr);
const char *get_ip(struct Client *client);

#endif
