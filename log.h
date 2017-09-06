#ifndef LOG_H_
#define LOG_H_

#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define LOG_FILE_NAME "log.txt"

FILE *log_file;

char *get_current_time(char *buffer, int size);
void log_msg(const char *message);
void log_client(const char *client_ip, const char *message);
void close_log_file();
void create_log_file();


#endif
