#include "client.h"

int on_headers_complete(http_parser *parser) {
    struct Client *client = parser->data;
    const char *method = http_method_str((enum http_method) parser->method);
    
    client->request.method = malloc(sizeof(method));
    strcpy(client->request.method, method);
    client->request.http_major = parser->http_major;
    client->request.http_minor = parser->http_minor;
    
    return 0;
}

int on_url(http_parser *parser, const char *at, size_t length) {
    struct Client *client = parser->data;
    
    client->request.url = malloc(char_size * length + 1);
    strncpy(client->request.url, at, length);
    client->request.url[length] = '\0';
    
    return 0;
}

int on_header_field(http_parser *parser, const char *at, size_t length) {
    return 0;
}

int on_header_value(http_parser *parser, const char *at, size_t length) {
    return 0;
}

int on_body(http_parser *parser, const char *at, size_t length) {
    struct Client *client = parser->data;
    
    client->request.body = malloc(char_size * length + 1);
    strncpy(client->request.body, at, length);
    client->request.body[length] = '\0';
    
    return 0;
}

void add_client(const int connfd, struct sockaddr_in *cliaddr) {
     clients[connfd] = (struct Client *) malloc (sizeof(struct Client));
     init_client(clients[connfd], connfd, cliaddr);
     log_client(clients[connfd], "connected");   
}

void remove_client(const int connfd) {
    log_client(clients[connfd], "disconnected");
    
    free(clients[connfd]->request.method);
    free(clients[connfd]->request.url);
    free(clients[connfd]->request.body);
    for (size_t i = 0; i < MAX_HEADER_COUNT; i++) {
        free(clients[connfd]->request.headers[i]);
    }

    free(clients[connfd]);
}

int process_message(struct Client *client) {
    char msg_buffer[MAX_LEN];

    http_parser *parser = malloc(sizeof(http_parser));
    http_parser_init(parser, HTTP_REQUEST);
    parser->data = client;

    size_t len = MAX_LEN, nparsed;
    ssize_t recved;

    recved = recv(client->connfd, msg_buffer, len, 0);
    msg_buffer[recved] = '\0';
    printf("%s\n", msg_buffer);

    if (recved < 0) {
        log_client(client, "Error recv");
    } else if (recved == 0) {
        return CLIENT_DISCONNECT;
    }
    
    nparsed = http_parser_execute(parser, &settings, msg_buffer, recved);
    
    if (parser->upgrade) {
        log_client(client, "Error parser->upgrade");
    } else if (nparsed != recved) {
        log_client(client, "Error nparsed != recved");
    }
    
    free(parser);

    return CLIENT_DISCONNECT;
}

void init_parser_settings() {
    settings.on_url = on_url;
    settings.on_header_field = on_header_field;
    settings.on_header_value = on_header_value;
    settings.on_body = on_body;
    settings.on_headers_complete = on_headers_complete;
}

void init_client(struct Client *client, int connfd, struct sockaddr_in *cliaddr) {
    char_size = sizeof(char);
    memset(client, 0, sizeof(struct Client));
    client->connfd = connfd;
    strcpy(client->ip, inet_ntoa(cliaddr->sin_addr));
}

char *get_info(struct Client *client, char *buffer) {
    if (sprintf(buffer, "%s %d", client->ip, client->connfd) < 0) {
        return NULL;
    }

    return buffer;
}
