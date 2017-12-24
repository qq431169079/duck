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

#include "precgi.h"
#include "lib/http_parser.h"

#define MAX_LEN 4096
#define MAX_HEADER_COUNT 64
#define LOG_BUF_SIZE 256

#ifndef FD_SETSIZE
#define FD_SETSIZE 1024
#endif

typedef struct {
  int meta_var; 
  char *cgi_env[CGI_META_VARIABLE_COUNT];
    
  char *body;
} http_request;

typedef struct {
  int connfd;
  char ip[128];

  http_request request;
} http_connection;


void init_http(http_connection *con[]);

char log_buffer[LOG_BUF_SIZE];

// Connections variable is defined in main, make local pointer points to it
void get_connection();
void init_parser_settings();

// Parsing
int on_message_complete(http_parser *parser);
int on_headers_complete(http_parser *parser);
int on_body(http_parser *parser, const char *at, size_t length);
int on_url(http_parser *parser, const char *at, size_t length);
int on_header_field(http_parser *parser, const char *at, size_t length);
int on_header_value(http_parser *parser, const char *at, size_t length);
int parse_http(http_connection *connection);

int send_n(size_t connfd, const char *message, size_t bytes_to_read, int flag);
int connection_handler(http_connection *connection);

// Connection
void add_connection(const int connfd, struct sockaddr_in *cliaddr);
void remove_connection(const int connfd);
void init_connection(http_connection *connection, int connfd, struct sockaddr_in *cliaddr);

#endif
