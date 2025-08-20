#include <arpa/inet.h> // sockaddr_in, inet_addr

#ifndef TCP_H
#define TCP_H


void server(char *address, int port, int buff_size);
void bind_listen_server(int server_fd, struct sockaddr_in *server_addr, int port);
void client_handler(int server_fd, int client_fd, struct sockaddr_in *client_addr, int buff_size);

#endif