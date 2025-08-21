#include <arpa/inet.h> // sockaddr_in, inet_addr

#ifndef TCP_H
#define TCP_H

typedef struct {
    char remote_host[INET_ADDRSTRLEN];
} Socket;
extern Socket client_socket;   // declaration (no memory allocated here)

typedef struct
{
    void (*once)(char *data);
    void (*on)(char *data);
    // void (*error)();
    void (*close)();
    void (*end)();
}EventListeners;
extern EventListeners event_listeners;

int server(const char *address, int port, int buffer_size);
void start_server(int server_fd);
void bind_server(int server_fd, struct sockaddr_in *server_addr, int port);
void communication_handler(int server_fd, int client_fd, struct sockaddr_in *client_addr, int buff_size);
void set_event_listeners(
    void(*once)(char *),
    void(*on)(char *),
    void(*close)(),
    void(*end)()
);

void handle_once_event();
void handle_on_event();
void handle_close_event();
void handle_end_event();

// private functions
static void convert_binary_ip_to_v4(struct sockaddr_in *client_addr, char *buffer, size_t buffer_len);
static void clean_up_zombies(int signo);
static void DEFAULT_ONCE_EVENT(char *data);     // once event default function (if user didn't define this)
static void DEFAULT_ON_EVENT(char *data);       // on event default function (if user didn't define this)
static void DEFAULT_CLOSE_EVENT();        // close event default function (if user didn't define this)
static void DEFAULT_END_EVENT();          // end event default function (if user didn't define this)
// static void DEFAULT_ERROR_EVENT();   // error event default function (if user didn't define this)
#endif