#include "log.h"

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
    fprintf(log_file, "%s %s %s\n", get_current_time(), message, strerror(errno));
    fflush(log_file);
}

void log_client(struct Client *client, const char *message) {
    fprintf(log_file, "%s %s %s %s\n", get_current_time(), 
                get_info(client, log_buffer), message, strerror(errno));
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
