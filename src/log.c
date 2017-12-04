#include "log.h"

FILE *log_file;

const char *log_http_request;
const char *log_http_connection;

char log_info_buffer[MAX_LOG_LEN];
char log_buffer[MAX_LOG_LEN];

const http_connection **connections;

void init_log(const http_connection **con) {
  create_log_file();
  log_get_connection(con);
}

void log_get_connection(const http_connection **con) {
    connections = con;
}

char *get_current_time() {
    static char buffer[26];
    time_t timer;
    struct tm* time_info;
    time(&timer);
    time_info = localtime(&timer);
  
    strftime(buffer, 26, "%H:%M:%S %D", time_info);
    return buffer;
}

void log_msg(const char *message) {
    fprintf(log_file, "%s %s\n", get_current_time(), message);
    fflush(log_file);
}

void log_connection(const int connfd, const char *message) {
    get_full_info(connfd);

    fprintf(log_file, "%s %s %s\n", get_current_time(), 
                log_info_buffer, message);
    fflush(log_file);
}

void close_log_file() {
    if (fclose(log_file) == EOF) {
        log_msg("Error closing log file");
    } 
}

void create_log_file() {
    if ((log_file = fopen(LOG_FILE_NAME, "w")) == NULL) {
        perror("Error creating log file"), exit(EXIT_FAILURE);
    }
}

const char *get_full_info(const int connfd) {
  const http_connection *connection = connections[connfd];
  if (sprintf(log_info_buffer, "%s %d %s %s", 
        connection->ip,  connection->connfd, 
        connection->request.url, connection->request.method) < 0) {                       return NULL;
  }

  return log_buffer;                                                                
}                                                                                     

const char *get_info(const int connfd) {
  const http_connection *connection = connections[connfd];
  if (sprintf(log_info_buffer, "%s %d", connection->ip, connection->connfd) < 0) {       
    return NULL;
  }

  return log_buffer;                                                                
}                                                                                     

