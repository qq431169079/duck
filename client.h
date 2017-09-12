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

typedef struct Request Request;
typedef struct Client Client;
typedef struct Response Response;

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

Client *clients[FD_SETSIZE];

http_parser_settings settings;
void init_parser_settings();
int on_message_complete(http_parser *parser);
int on_headers_complete(http_parser *parser);
int on_url(http_parser *parser, const char *at, size_t length);
int on_header_field(http_parser *parser, const char *at, size_t length);
int on_header_value(http_parser *parser, const char *at, size_t length);
int on_body(http_parser *parser, const char *at, size_t length);

size_t get_digits_of_length(size_t partial_content_length);
int process_response(Client *client);
int process_message(Client *client);

void add_client(const int connfd, struct sockaddr_in *cliaddr);
void remove_client(const int connfd);
void init_client(Client *client, int connfd, struct sockaddr_in *cliaddr);
char *get_info(Client *client, char *buffer);

#endif
