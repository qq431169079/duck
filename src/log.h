#ifndef LOG_H_
#define LOG_H_

#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "http.h"

#define LOG_FILE_NAME "log.txt"
#define MAX_LOG_LEN 256


char *get_current_time();

static const char *get_full_info(const int connfd);
static const char *get_info(const int connfd); 

void log_msg(const char *message);
void log_connection(const int connfd, const char *message);

void init_log(const http_connection **con);

void log_get_connection(const http_connection **con);
void close_log_file();
void create_log_file();


#endif
