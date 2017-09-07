#ifndef CLIENT_H_
#define CLIENT_H_

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "log.h"

#define MAX_LEN 4096
#define MAX_IP 128
#define MAX_INFO_LEN 256

#define CLIENT_DISCONNECT -1
#define ERROR_WRITE -2
#define ERROR_READ -3

struct Client {
    int connfd;
    char ip[MAX_IP];

    int byte_to_read;
    char msg_buffer[MAX_LEN];
};

int process_message(struct Client *client);
int read_message(struct Client *client);
int write_message(struct Client *client); 
void set_client(struct Client *client, int connfd, struct sockaddr_in *cliaddr);
char *get_info(struct Client *client, char *buffer);

#endif
