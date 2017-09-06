#include "client.h"

void set_ip(struct Client *client, struct sockaddr_in *cliaddr) {
    strcpy(client->ip, inet_ntoa(cliaddr->sin_addr));
}

const char *get_ip(struct Client *client) {
    return client->ip;
}
