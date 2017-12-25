#include "log.h"
#include "http.h"

static http_connection **connections;

http_parser_settings settings;

void init_http(http_connection *con[]) {
  init_parser_settings();
  get_connection(con);
}

void get_connection(http_connection *con[]) {
  connections = con;
}

int on_message_complete(http_parser *parser) {
  http_connection *connection = parser->data;

  set_meta_variable(REMOTE_ADDR, connection->request.cgi_env,
      connection->ip,  strlen(connection->ip)); 

  // Init the cgi meta-variables that are not given by the request
  fill_cgi_meta_variables(connection->request.cgi_env);

  int i = 0;
  for ( ; i < CGI_META_VARIABLE_COUNT; ++i) {
    if (connection->request.cgi_env[i]) {
      printf("%s\n", connection->request.cgi_env[i]);
    }
  }

  return 0;
}

int on_headers_complete(http_parser *parser) {
  http_connection *connection = parser->data;
  const char *method = http_method_str((enum http_method) parser->method);
  
  set_meta_variable(REQUEST_METHOD, 
      connection->request.cgi_env, method, strlen(method));

  // HTTP/1.1\0  9 characters
  char buf[9];
  sprintf(buf, "HTTP/%d.%d", parser->http_major, parser->http_minor);
  set_meta_variable(SERVER_PROTOCOL,
      connection->request.cgi_env, buf, 9);

  return 0;
}

static int parse_pathinfo_scriptname(const char *uri, http_connection *connection) {
  size_t uri_len = strlen(uri);

  if (uri_len == 1 && uri[0] == '/') {
    set_meta_variable(PATH_INFO,   connection->request.cgi_env,
        "/",  1);  
  } else if (uri_len > 4 && strncmp(uri, "/cgi", 4) == 0) {
    set_meta_variable(SCRIPT_NAME, connection->request.cgi_env,
        "/cgi", 4);
    set_meta_variable(PATH_INFO,   connection->request.cgi_env,
        uri + 4,   uri_len - 4);
  } else {
    return -1;
  }
}

// http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]
int on_url(http_parser *parser, const char *at, size_t length) {
  http_connection *connection = parser->data;
  struct http_parser_url u;
  http_parser_url_init(&u);

  if (http_parser_parse_url(at, length, 0, &u) == 0) {
    // Get PATH_INFO and SCRIPT_NAME
    char uri[500];
    memset(uri, 0, 500);
    strncpy(uri, at + u.field_data[UF_PATH].off, u.field_data[UF_PATH].len);
    if (parse_pathinfo_scriptname(uri, connection) < 0) {
      log_msg("on_url parse_path_info_scriptname failed");
      return -1;
    }
    
    // Get QUERY_STRING
    if (u.field_data[UF_QUERY].len > 0) {
      set_meta_variable(QUERY_STRING, connection->request.cgi_env,
          at + u.field_data[UF_QUERY].off, u.field_data[UF_QUERY].len);
    }
  } else {
    log_msg("on_url http_parser_parser_url failed");
    return -1;
  }

  return 0;
}

int on_header_field(http_parser *parser, const char *at, size_t length) {
  http_connection *connection = parser->data;
  connection->request.meta_var = parse_header_field(at, length);
  return 0;
}

int on_header_value(http_parser *parser, const char *at, size_t length) {
  http_connection *connection = parser->data;
  // The server ignores some headers
  if (connection->request.meta_var >= 0) {
    set_meta_variable(connection->request.meta_var, 
        connection->request.cgi_env, at, length);
  }
  return 0;
}

int on_body(http_parser *parser, const char *at, size_t length) {
  if (length == 0) {
    return 0;
  }
  http_connection *connection = parser->data;
  connection->request.body = (char *)malloc(length + 1);
    
  strncpy(connection->request.body, at, length);
  connection->request.body[length] = '\0';
  return 0;
}

// Connection
void add_connection(const int connfd, struct sockaddr_in *cliaddr) {
    connections[connfd] = (http_connection *) malloc (sizeof(http_connection));
    init_connection(connections[connfd], connfd, cliaddr);
}

void remove_connection(const int connfd) {
    log_connection(connfd, "disconnected");
    
    http_connection *con = connections[connfd];
    
    free_meta_variables(con->request.cgi_env);
    free(con->request.body);
    free(con);
}

int send_n(size_t connfd, const char *message, size_t bytes_to_send, int flag) {
    const char *ptr;
  
    if ((ptr = message) == 0) {
        log_connection(connfd, "message is empty");
        return 0;
    }
    for (ssize_t bytes_sent = 0; bytes_to_send > 0; ) {
        if ((bytes_sent = send(connfd, ptr, bytes_to_send, flag)) < 0) {
            if (bytes_sent < 0 && errno == EINTR) {
                bytes_sent = 0;
            } else {
                log_connection(connfd, "send_n error");
                return -1;
            }
        }
        bytes_to_send -= bytes_sent;
        ptr += bytes_sent;
    }

    return 0;
}

/*
ssize_t read_file(http_connection *connection, const char *path_to_file, char *body, enum file_open_mode f_mode) {
    FILE *file;
    char *buf;
    ssize_t body_size;
    char *open_mode;
    
    switch(f_mode) {
        case OPEN_BINARY: open_mode = "rb"; break;
        case OPEN_TEXT:   open_mode = "r";  break;
        default:     log_connection(connection->connfd, "Unknown file_type"); return -1;
    }

    if ((file = fopen(path_to_file, open_mode)) == NULL) {
        log_connection(connection->connfd, "Error open file");
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
        log_connection(connection->connfd, "Error close file");
        return -1;
    }

    return body_size;
}*/

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
        log_connection(connection->connfd, "Error recv");
        return -1;
    } else if (recved == 0) {
        return 0;
    }
    //printf("%s\n", msg_buffer);
    nparsed = http_parser_execute(parser, &settings, msg_buffer, recved);
    
    if (parser->upgrade) {
        log_connection(connection->connfd, "Error parser->upgrade");
        return -1;
    } else if (nparsed != recved) {
        log_connection(connection->connfd, "Error nparsed != recved");
        return -1;
    }
    free(parser);

    return 0;
}

int connection_handler(http_connection *connection) {
  int status;
  if ((status = parse_http(connection)) < 0) {
    log_connection(connection->connfd, "parser_http in http.c failed");
    return -1;
  }
  
  // Log the connection
  log_connection(connection->connfd, "");  
  
  // Start CGI 
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

