#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "precgi.h"
#include "log.h"

short server_port;
char *server_port_str;

static const char *cgi_meta_variable_str_map[CGI_META_VARIABLE_COUNT] = {
  [CONTENT_TYPE]            = "CONTENT_TYPE",
  [CONTENT_LENGTH]          = "CONTENT_LENGTH",
  [GATEWAY_INTERFACE]       = "GATEWAY_INTERFACE",
  [PATH_INFO]               = "PATH_INFO",
  [QUERY_STRING]            = "QUERY_STRING",
  [REMOTE_ADDR]             = "REMOTE_ADDR",
  [REQUEST_METHOD]          = "REQUEST_METHOD",
  [REMOTE_HOST]             = "REMOTE_HOST",
  [SCRIPT_NAME]             = "SCRIPT_NAME",
  [SERVER_PORT]             = "SERVER_PORT",
  [SERVER_PROTOCOL]         = "SERVER_PROTOCOL",
  [SERVER_SOFTWARE]         = "SERVER_SOFTWARE",
  [HTTP_ACCEPT]             = "HTTP_ACCEPT",
  [HTTP_REFERER]            = "HTTP_REFERER",
  [HTTP_ACCEPT_ENCODING]    = "HTTP_ACCEPT_ENCODING",
  [HTTP_ACCEPT_LANGUAGE]    = "HTTP_ACCEPT_LANGUAGE",
  [HTTP_ACCEPT_CHARSET]     = "HTTP_ACCEPT_CHARSET",
  [HTTP_HOST]               = "HTTP_HOST",
  [HTTP_COOKIE]             = "HTTP_COOKIE",
  [HTTP_USER_AGENT]         = "HTTP_USER_AGENT",
  [HTTP_CONNECTION]         = "HTTP_CONNECTION",
};

static const char *http_meta_map[CGI_META_VARIABLE_COUNT] = {
  [CONTENT_TYPE]            = "Content-Type",
  [CONTENT_LENGTH]          = "Content-Length",
  [HTTP_ACCEPT]             = "Accept",
  [HTTP_REFERER]            = "Referer",
  [HTTP_ACCEPT_ENCODING]    = "Accept-Encoding",
  [HTTP_ACCEPT_LANGUAGE]    = "Accept-Language",
  [HTTP_ACCEPT_CHARSET]     = "Accept-Charset",
  [HTTP_HOST]               = "Host",
  [HTTP_COOKIE]             = "Cookie",
  [HTTP_USER_AGENT]         = "User-Agent",
  [HTTP_CONNECTION]         = "Connection",
};

int init_cgi_settings(short port) {
  set_server_port(port);
  return 0;
}

int remove_cgi_settings() {
  free(server_port_str);
  return 0;
}

int set_server_port(short port) {
  server_port = port;
  server_port_str = (char *)malloc(6);
  sprintf(server_port_str, "%d", server_port);
  return 0;
}

const char *get_server_port_str() {
  return server_port_str;
}

const char *get_meta_var_str(enum cgi_meta_variable var) {
  if (var < 0 || var >= CGI_META_VARIABLE_COUNT) {
    log_msg("Server error: Function get_meta_var_str Error var out of range");
  }
  return cgi_meta_variable_str_map[var];
}

const char *get_meta_var_http_str(enum cgi_meta_variable var) {
  if (var < 0 || var >= CGI_META_VARIABLE_COUNT) {
    log_msg("Server error: Function get_meta_var_http_str Error var out of range");
  }
  return http_meta_map[var];
}

int set_meta_variable(enum cgi_meta_variable var, char **cgi_env, 
    const char *value, int value_len) {
  
  // Including the size of "=" and "\0"
  size_t total_len = strlen(get_meta_var_str(var)) + 1 + value_len + 1;
  cgi_env[var] = (char *)malloc(total_len);
  
  if (snprintf(cgi_env[var], total_len, "%s=%s", 
        get_meta_var_str(var), value) < 0) {
    return -1;
  }
  
  return 0;
}

int free_meta_variables(char **cgi_env) {
  size_t i = 0;

  for ( ; i < CGI_META_VARIABLE_COUNT; ++i) {
    free(cgi_env[i]);
  }

  return 0;
}

int fill_cgi_meta_variables(char **cgi_env) {
  set_meta_variable(GATEWAY_INTERFACE, cgi_env, 
      DEFAULT_CGI_PROTOCOL,    strlen(DEFAULT_CGI_PROTOCOL));
  set_meta_variable(SERVER_SOFTWARE,   cgi_env,
      DEFAULT_SERVER_SOFTWARE, strlen(DEFAULT_SERVER_SOFTWARE));
  set_meta_variable(SERVER_PORT,       cgi_env,
      server_port_str,         strlen(server_port_str));

  // Initialize the meta-variables that are null, but leave the value empty
  size_t i = 0;
  for ( ; i < CGI_META_VARIABLE_COUNT; ++i) {
    if (cgi_env[i] == 0) {
      cgi_env[i] = (char *)malloc(strlen(get_meta_var_str(i)) + 1 + 1);
      sprintf(cgi_env[i], "%s=", get_meta_var_str(i));
    }
  }

  return 0;
}

enum cgi_meta_variable parse_header_field(const char *at, size_t len) {
  size_t i = 0;
  const char *meta_var_as_http;
  for ( ; i < CGI_META_VARIABLE_COUNT; ++i) {
    meta_var_as_http = get_meta_var_http_str(i);
    if (meta_var_as_http && strncmp(at, meta_var_as_http, len) == 0) {
      return i;
    }
  }
  return -1;
}


