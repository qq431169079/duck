#ifndef LOG_H_
#define LOG_H_

#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "client.h"

#define LOG_FILE_NAME "log.txt"
#define MAX_LOG_LEN 256

struct Client;
FILE *log_file;

char log_buffer[MAX_LOG_LEN];

char *get_current_time();
void log_msg(const char *message);
void log_client(struct Client *client, const char *message);
void close_log_file();
void create_log_file();


#endif
