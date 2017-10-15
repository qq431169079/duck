#ifndef HTTP_H_
#define HTTP_H_

#define _GNU_SOURCE

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "lib/http_parser.h"
#include "resource_manager.h"
#include "log.h"

enum file_open_mode {
    OPEN_BINARY = 0
,   OPEN_TEXT   = 1
};

#define MAX_LEN 4096
#define MAX_HEADER_COUNT 64
#define LOG_BUF_SIZE 256

#ifndef FD_SETSIZE
#define FD_SETSIZE 1024
#endif

typedef struct {
    char *method;
    char *url;
    char *headers[MAX_HEADER_COUNT];
    char *body;
    unsigned short http_major;
    unsigned short http_minor;
} http_request;

typedef struct {
    int connfd;
    char ip[128];

    http_request request;
} http_connection;

http_connection *connections[FD_SETSIZE];
http_parser_settings settings;
char log_buffer[LOG_BUF_SIZE];

void init_parser_settings();

int on_message_complete(http_parser *parser);
int on_headers_complete(http_parser *parser);

int on_body(http_parser *parser, const char *at, size_t length);
int on_url(http_parser *parser, const char *at, size_t length);
int on_header_field(http_parser *parser, const char *at, size_t length);
int on_header_value(http_parser *parser, const char *at, size_t length);

int send_n(size_t connfd, const char *message, size_t bytes_to_read, int flag);

ssize_t read_file(http_connection *connection, const char *path_to_file, char *body, enum file_open_mode f_mode);

int fetch_files(http_connection *connection);
int parse_http(http_connection *connection);
int construct_response(http_connection *connection, char *http_info, size_t *http_info_size, char *body, size_t *body_size);
int send_response(http_connection *connection, const char *http_info, const size_t http_info_size, const char *body, const size_t body_size);

int process_response(http_connection *connection);
int connection_handler(http_connection *connection);

void add_connection(const int connfd, struct sockaddr_in *cliaddr);
void remove_connection(const int connfd);
void init_connection(http_connection *connection, int connfd, struct sockaddr_in *cliaddr);

const char *get_info(const http_connection *connection);

#endif
