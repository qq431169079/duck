#include "resource_manager.h"

unsigned long string_hash(const unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while (c = *(str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % RESOURCE_MAX_SIZE;
}

size_t get_index(resource_manager *resource, const unsigned char *key, int mode) {
    unsigned int index = string_hash(key) % RESOURCE_MAX_SIZE;
    for (int i = index - 1; i != index; i--) {
        i = (i == -1) ? RESOURCE_MAX_SIZE - 1 : i;
        if ((mode == RESOURCE_INSERT && resource->key_index_table[i] == 0) ||
            !(mode == RESOURCE_GET && strcmp(resource->key_index_table[i], key))) {
            return i;
        }
    }
    return -1;
}

void add_resource(resource_manager *resource, const unsigned char *key, const unsigned char *value) {
    if (resource->size == RESOURCE_MAX_SIZE) {
        log_msg("The resource manager is unable to add more resource, it has reached its capacity.");
        return ;
    }
    
    size_t index = get_index(resource, key, RESOURCE_INSERT);
    resource->key_index_table[index] = key;
    resource->index_value_table[index] = value;
    resource->size++;
}

const char * get_resource(resource_manager *resource, const unsigned char *key) {
    unsigned int index = get_index(resource, key, RESOURCE_GET);
    return resource->index_value_table[index];
}
