#include "client.h"

int process_message(struct Client *client) {
    int status = 0;
    if ((status = read_message(client)) == CLIENT_DISCONNECT) {
        return CLIENT_DISCONNECT;
    }
    write_message(client);

    return 0;
}

int read_message(struct Client *client) {
    int byte_read = 0;
    byte_read = read(client->connfd, client->msg_buffer, MAX_LEN);
    
    if (byte_read == 0) {
        return CLIENT_DISCONNECT;
    } else if (byte_read == -1) {
        log_msg("Error reading from client");
        return ERROR_READ;
    }

    client->byte_to_read += byte_read;

    return 0;
}

int write_message(struct Client *client) {
    int byte_written = write(client->connfd, client->msg_buffer, 
            client->byte_to_read);
    if (byte_written < client->byte_to_read) {
        ;
    } else if (byte_written == -1) {
        log_msg("Error writing to client");
        return ERROR_WRITE;
    }

    client->byte_to_read -= byte_written;

    return 0;
}

void set_client(struct Client *client, int connfd, struct sockaddr_in *cliaddr) {
    client->byte_to_read = 0;
    client->connfd = connfd;
    strcpy(client->ip, inet_ntoa(cliaddr->sin_addr));
}

char *get_info(struct Client *client, char *buffer) {
    if (sprintf(buffer, "%s %d", client->ip, client->connfd) < 0) {
        return NULL;
    }

    return buffer;
}
