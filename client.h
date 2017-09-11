#ifndef CLIENT_H_
#define CLIENT_H_

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "http_parser.h"
#include "log.h"

#ifndef MAX_LEN
#define MAX_LEN 4096
#endif 

#ifndef MAX_HEADER_COUNT
#define MAX_HEADER_COUNT 64
#endif 

#ifndef CLIENT_DISCONNECT
#define CLIENT_DISCONNECT -1
#endif

struct Response {


};

struct Request {
    char *method;
    char *url;
    char *headers[MAX_HEADER_COUNT];
    char *body;
    unsigned short http_major;
    unsigned short http_minor;
};

struct Client {
    int connfd;
    char ip[128];

    struct Request request;
    struct Response response;
};

size_t char_size;

struct Client *clients[FD_SETSIZE];

http_parser_settings settings;
void init_parser_settings();
int on_headers_complete(http_parser *parser);
int on_url(http_parser *parser, const char *at, size_t length);
int on_header_field(http_parser *parser, const char *at, size_t length);
int on_header_value(http_parser *parser, const char *at, size_t length);
int on_body(http_parser *parser, const char *at, size_t length);

void add_client(const int connfd, struct sockaddr_in *cliaddr);
void remove_client(const int connfd);
int process_message(struct Client *client);
void init_client(struct Client *client, int connfd, struct sockaddr_in *cliaddr);
char *get_info(struct Client *client, char *buffer);

#endif
