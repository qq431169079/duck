#include "client.h"

void add_client(const int connfd, struct sockaddr_in *cliaddr) {
     clients[connfd] = (struct Client *) malloc (sizeof(struct Client));
     init_client(clients[connfd], connfd, cliaddr);
     log_client(clients[connfd], "connected");   
}

void remove_client(const int connfd) {
    log_client(clients[connfd], "disconnected");
    free(clients[connfd]);
}

int process_message(struct Client *client) {
    int status = 0;

    status = read_message(client);
    if (status == CLIENT_DISCONNECT) {
        return status;
    } else if (status == ERROR_READ) {
        log_client(client, "Error reading from client");
        return status;
    }

    status = write_message(client);
    if (status == ERROR_WRITE) {
        log_client(client, "Error writing to client");
    } else if (status == INCOMPLETE_WRITE) {
        log_client(client, "Incomplete write to client");
    }

    return status;
}

int read_message(struct Client *client) {
    if (client->byte_to_write != 0) {
        return 0;
    }

    int byte_read = 0;
    byte_read = read(client->connfd, client->msg_buffer, MAX_LEN);

    printf("%s", client->msg_buffer);
    if (byte_read == 0) {
        return CLIENT_DISCONNECT;
    } else if (byte_read == -1) {
        return ERROR_READ;
    }

    client->byte_to_write += byte_read;
    return 0;
}

void forward_msg_buffer(struct Client *client, int offset) {
    for (int i = 0; i < client->byte_to_write; i++) {
        client->msg_buffer[i] = client->msg_buffer[i + offset];
    }
}

int write_message(struct Client *client) {
    int byte_written = 0;
    
    byte_written = write(client->connfd, client->msg_buffer, client->byte_to_write);
    if (byte_written < client->byte_to_write) {
        client->byte_to_write -= byte_written;
        forward_msg_buffer(client, byte_written);
        return INCOMPLETE_WRITE;
    } else if (byte_written == -1) {
        return ERROR_WRITE;
    }

    client->byte_to_write -= byte_written;
    return byte_written;
}

void init_client(struct Client *client, int connfd, struct sockaddr_in *cliaddr) {
    client->byte_to_write = 0;
    client->connfd = connfd;
    strcpy(client->ip, inet_ntoa(cliaddr->sin_addr));
}

char *get_info(struct Client *client, char *buffer) {
    if (sprintf(buffer, "%s %d", client->ip, client->connfd) < 0) {
        return NULL;
    }

    return buffer;
}
