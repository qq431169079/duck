#ifndef PRECGI_H_
#define PRECGI_H_

#define REQUEST_METHOD_LEN              15
#define SERVER_PROTOCOL_LEN             16
#define CONTENT_LENGTH_LEN              15
#define HTTP_REFERER_LEN                13                   
#define CGI_META_VARIABLE_COUNT         21    

#define DEFAULT_SERVER_SOFTWARE         "DUCK/1.1"
#define DEFAULT_SERVER_PROTOCOL         "HTTP/1.1"
#define DEFAULT_CGI_PROTOCOL            "CGI/1.1"
#define DEFAULT_CGI_PATH                "./cgi"

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

int init_cgi_settings(short port);
int remove_cgi_settings();
int set_server_port(short server_port);
int fill_cgi_meta_variables(char **cgi_env);
int set_meta_variable(enum cgi_meta_variable var, char **cgi_env,
                        const char *value, int value_len);
int free_meta_variables(char **cgi_env);
enum cgi_meta_variable parse_header_field(const char *at, size_t len);

#endif


