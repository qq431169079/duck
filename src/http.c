#include "http.h"

static const char *PATH_TO_INDEX = "../www/index.html";
//static const char *PATH_TO_IMAGE = "../www/images/liso_header.png";
static const char *HTTP_VERSION = "HTTP/1.1";

int on_message_complete(http_parser *parser) {
    return 0;
}

int on_headers_complete(http_parser *parser) {
    http_connection *connection = parser->data;
    const char *method = http_method_str((enum http_method) parser->method);
    
    connection->request.method = malloc(sizeof(method));
    strcpy(connection->request.method, method);
    connection->request.http_major = parser->http_major;
    connection->request.http_minor = parser->http_minor;
    
    return 0;
}

int on_url(http_parser *parser, const char *at, size_t length) {
    http_connection *connection = parser->data;
    
    connection->request.url = malloc(length + 1);
    strncpy(connection->request.url, at, length);
    connection->request.url[length] = '\0';
    
    return 0;
}

int on_header_field(http_parser *parser, const char *at, size_t length) {
    return 0;
}

int on_header_value(http_parser *parser, const char *at, size_t length) {
    return 0;
}

int on_body(http_parser *parser, const char *at, size_t length) {
    http_connection *connection = parser->data;
    
    connection->request.body = malloc(length + 1);
    strncpy(connection->request.body, at, length);
    connection->request.body[length] = '\0';
    
    return 0;
}

void add_connection(const int connfd, struct sockaddr_in *cliaddr) {
     connections[connfd] = (http_connection *) malloc (sizeof(http_connection));
     init_connection(connections[connfd], connfd, cliaddr);
     log_connection(get_info(connections[connfd]), "connected");   
}

void remove_connection(const int connfd) {
    log_connection(get_info(connections[connfd]), "disconnected");
    
    free(connections[connfd]->request.method);
    free(connections[connfd]->request.url);
    free(connections[connfd]->request.body);
    for (size_t i = 0; i < MAX_HEADER_COUNT; i++) {
        free(connections[connfd]->request.headers[i]);
    }

    free(connections[connfd]);
}

size_t get_digits_of_length(size_t partial_length) {
    size_t count = 0;
    for (int i = partial_length; i != 0; i /= 10) {
        count++;
    }
    
    if ((count == 1 && partial_length >=   9)  ||
        (count == 2 && partial_length >=   98) ||
        (count == 3 && partial_length >=  997) ||
        (count == 4 && partial_length >= 9996))  {
        count++;
    } 

    return count;
}

int send_n(size_t connfd, const char *message, size_t bytes_to_send, int flag) {
    const char *ptr;
    
    ptr = message;
    for (ssize_t bytes_sent = 0; bytes_to_send > 0; ) {
        if ((bytes_sent = send(connfd, ptr, bytes_to_send, flag)) < 0) {
            if (bytes_sent < 0 && errno == EINTR) {
                bytes_sent = 0;
            } else {
                return -1;
            }
        }
        bytes_to_send -= bytes_sent;
        ptr += bytes_sent;
    }

    return 0;
}

ssize_t read_file(http_connection *connection, const char *path_to_file, char *body, enum file_type f_type) {
    FILE *file;
    char *buf;
    ssize_t body_size;
    char *open_mode;
    
    switch(f_type) {
        case BINARY: open_mode = "rb"; break;
        case TEXT:   open_mode = "r";  break;
        default:     log_connection(get_info(connection), "Unknown file_type"); return -1;
    }

    if ((file = fopen(path_to_file, open_mode)) == NULL) {
        log_connection(get_info(connection), "Error open file");
        return -1;
    }
    
    body_size = 0;
    buf = NULL;
    for (size_t nbyte_read = 0, len = 0; nbyte_read != -1; ) {
        nbyte_read = getline(&buf, &len, file);
        if (nbyte_read != -1) {
            body_size += nbyte_read;
            strcat(body, buf);
        }
    }
    free(buf);

    if (fclose(file) == EOF) {
        log_connection(get_info(connection), "Error close file");
        return -1;
    }

    return body_size;
}

int parse_http(http_connection *connection) {
    char msg_buffer[MAX_LEN];
    size_t len = MAX_LEN, nparsed;
    ssize_t recved;

    http_parser *parser = malloc(sizeof(http_parser));
    http_parser_init(parser, HTTP_REQUEST);
    parser->data = connection;

    recved = recv(connection->connfd, msg_buffer, len, 0);
    msg_buffer[recved] = '\0';

    if (recved < 0) {
        log_connection(get_info(connection), "Error recv");
        return -1;
    } else if (recved == 0) {
        return 0;
    }
    
    nparsed = http_parser_execute(parser, &settings, msg_buffer, recved);
    
    if (parser->upgrade) {
        log_connection(get_info(connection), "Error parser->upgrade");
        return -1;
    } else if (nparsed != recved) {
        log_connection(get_info(connection), "Error nparsed != recved");
        return -1;
    }
    free(parser);
    
    return 0;
}

int fetch_files(http_connection *connection) {
    return 0;
}

int construct_response(http_connection *connection, char *http_info, size_t *http_info_size, char *body, size_t *body_size) {
    if ((*body_size = read_file(connection, PATH_TO_INDEX, body, TEXT)) < 0) {
        return -1;
    }
    
    char *target = http_info;
    target += sprintf(target, "%s %d %s\n", HTTP_VERSION, 200, "OK");
    target += sprintf(target, "%s: %s\n", "Content-Type", "text/html");
    target += sprintf(target, "%s: %lu\n", "Content-Length", *body_size);
    target += sprintf(target, "%s: %s\n", "Connection", "close");
    target += sprintf(target, "\n");

    *http_info_size = target - http_info;
    return 0;
}

int send_response(http_connection *connection, const char *http_info, const size_t http_info_size, const char *body, const size_t body_size) {
    if (send_n(connection->connfd, http_info, http_info_size, 0) == -1) {
        log_connection(get_info(connection), "send error");
        return -1;
    }

    if (send_n(connection->connfd, body, body_size, 0) == -1) {
        log_connection(get_info(connection), "send error");
        return -1;
    }
    
    return 0;
}

int process_response(http_connection *connection) {
    size_t body_size = 0;
    size_t http_info_size = 0;
    char body[20000] = { 0 };   // TODO: how to dynamic allocate just enough space?
    char http_info[1024];        // TODO: same as above

    if (fetch_files(connection) == -1) {
        return -1;
    }
    if (construct_response(connection, http_info, &http_info_size, body, &body_size) == -1) {
        return -1;
    }
    if (send_response(connection, http_info, http_info_size, body, body_size) == -1) {
        return -1;
    }
    return 0;
}

int connection_handler(http_connection *connection) {
    int status;
    if ((status = parse_http(connection)) == -1) {
        return -1;
    }
    if ((status = process_response(connection)) == -1) {
        return -1;
    }
    return 0;
}

void init_parser_settings() {
    settings.on_url = on_url;
    settings.on_header_field = on_header_field;
    settings.on_header_value = on_header_value;
    settings.on_body = on_body;
    settings.on_headers_complete = on_headers_complete;
    settings.on_message_complete = on_message_complete;
}

void init_connection(http_connection *connection, int connfd, struct sockaddr_in *cliaddr) {
    memset(connection, 0, sizeof(http_connection));
    connection->connfd = connfd;
    strcpy(connection->ip, inet_ntoa(cliaddr->sin_addr));
}

const char *get_info(const http_connection *connection) {
    if (sprintf(log_buffer, "%s %d", connection->ip, connection->connfd) < 0) {
        return NULL;
    }

    return log_buffer;
}