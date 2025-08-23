#include <arpa/inet.h> // sockaddr_in, inet_addr

#ifndef TCP_H
#define TCP_H

typedef void (*close_connection_event) (void);  // for end and close connection event
typedef void (*on_data_event) (char*, int); // type of on_data and once_data handler

typedef struct
{
    on_data_event once;
    on_data_event on_data;
    // void (*error)();
    close_connection_event on_close;
    close_connection_event on_end;
} EventListeners;

extern EventListeners event_listeners;

typedef struct {
    char remote_host[INET_ADDRSTRLEN];
    ssize_t (*emit)(int client_fd, const void *response, int flag);
    EventListeners events;
    
} Socket;
extern Socket client_socket;   // declaration (no memory allocated here)

Socket* server(int buffer_size);
void start_server(Socket *socket, const char *address, int port);
void bind_server(int server_fd, struct sockaddr_in *server_addr, int port);
void communication_handler(int server_fd, struct sockaddr_in *client_addr);

// void handle_once_event();
// void handle_on_event();
// void handle_close_event();
// void handle_end_event();

#endif