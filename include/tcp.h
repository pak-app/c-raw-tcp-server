#include <arpa/inet.h> // sockaddr_in, inet_addr

#ifndef TCP_H
#define TCP_H

struct Socket;

typedef void (*close_connection_event) (struct Socket*);  // for end and close connection event
typedef void (*on_data_event) (struct Socket*, char*, int); // type of on_data and once_data handler

typedef struct
{
    on_data_event once;
    on_data_event on_data;
    // void (*error)();
    close_connection_event on_close;
    close_connection_event on_end;
} EventListeners;

extern EventListeners event_listeners;

typedef struct Socket {
    char remote_host[INET_ADDRSTRLEN];
    ssize_t (*emit)(const void *response, int flag);
    EventListeners events;
    int client_fd;
    
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