#ifndef CGI_H_
#define CGI_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define REQUEST_METHOD_LEN        15
#define SERVER_PROTOCOL_LEN       16
#define CONTENT_LENGTH_LEN        15
#define HTTP_REFERER_LEN          13                   

#define CGI_PATH              "./cgi"
#define CGI_VERSION           "CGI/1.1"
#define CGI_META_VARIABLE_COUNT      21    

enum cgi_meta_variable {
  CONTENT_TYPE                = 0,
  CONTENT_LENGTH              = 1,
  GATEWAY_INTERFACE           = 2,
  PATH_INFO                   = 3,
  QUERY_STRING                = 4,
  REMOTE_ADDR                 = 5,
  REQUEST_METHOD              = 6,
  REMOTE_HOST                 = 7,
  SCRIPT_NAME                 = 8,
  SERVER_PORT                 = 9,
  SERVER_PROTOCOL             = 10,
  SERVER_SOFTWARE             = 11,
  HTTP_ACCEPT                 = 12,
  HTTP_REFERER                = 13,
  HTTP_ACCEPT_ENCODING        = 14,
  HTTP_ACCEPT_LANGUAGE        = 15,
  HTTP_ACCEPT_CHARSET         = 16,
  HTTP_HOST                   = 17,
  HTTP_COOKIE                 = 18,
  HTTP_USER_AGENT             = 19,
  HTTP_CONNECTION             = 20,
}; 

const char *get_meta_var_http_str(enum cgi_meta_variable var);
const char *get_meta_var_str(enum cgi_meta_variable var);
int fill_cgi_meta_variables(char **cgi_env);
int set_meta_variable(enum cgi_meta_variable var, char **cgi_env,
                        const char *value, int value_len);
int free_meta_variables(char **cgi_env);
enum cgi_meta_variable parse_header_field(const char *at, size_t len);
#endif


