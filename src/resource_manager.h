#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <string.h>

#include "log.h"

#define RESOURCE_MAX_SIZE 7
#define RESOURCE_INSERT 0
#define RESOURCE_GET 1

typedef struct {
  const unsigned char *key_index_table[RESOURCE_MAX_SIZE];
  const unsigned char *index_value_table[RESOURCE_MAX_SIZE];
  
  
  unsigned int size;
} resource_manager;

unsigned long string_hash(const unsigned char *str);

size_t get_index(resource_manager *resource, const unsigned char *key, int mode);

void add_resource(resource_manager *resource, const unsigned char *key, const unsigned char *value);

const char * get_resource(resource_manager *resource, const unsigned char *key);

#endif
