#include "log.h"

char *get_current_time(char *buffer, int size) {
    time_t timer;
    struct tm* time_info;
    time(&timer);
    time_info = localtime(&timer);
  
    strftime(buffer, size, "%H:%M:%S %D", time_info);
    return buffer;
}

void log_msg(const char *message) {
    char buffer[26];
    fprintf(log_file, "%s %s %s\n", get_current_time(buffer, 26), message, strerror(errno));
    fflush(log_file);
}

void log_client(const char *client_ip, const char *message) {
    char buffer[26];
    fprintf(log_file, "%s %s %s\n", get_current_time(buffer, 26), client_ip, message);
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
