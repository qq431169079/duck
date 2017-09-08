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
#define INCOMPLETE_WRITE -4

struct Client {
    int connfd;
    char ip[MAX_IP];

    int byte_to_write;
    char msg_buffer[MAX_LEN];
};

struct Client *clients[FD_SETSIZE];

void add_client(const int connfd, struct sockaddr_in *cliaddr);
void remove_client(const int connfd);
void forward_msg_buffer(struct Client *client, int offset);
int process_message(struct Client *client);
int read_message(struct Client *client);
int write_message(struct Client *client); 
void init_client(struct Client *client, int connfd, struct sockaddr_in *cliaddr);
char *get_info(struct Client *client, char *buffer);

#endif
