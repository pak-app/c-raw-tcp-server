#include <arpa/inet.h> // sockaddr_in, inet_addr

#ifndef TCP_H
#define TCP_H

typedef struct {
    char remote_host[INET_ADDRSTRLEN];
    ssize_t (*emit)(int client_fd, const void *response, size_t response_len, int flag);
} Socket;
extern Socket client_socket;   // declaration (no memory allocated here)

typedef struct
{
    void (*once)(char *data, int bytes);
    void (*on_data)(char *data, int bytes);
    // void (*error)();
    void (*on_close)();
    void (*on_end)();
}EventListeners;
extern EventListeners event_listeners;

int server(const char *address, int port, int buffer_size);
void start_server(int server_fd);
void bind_server(int server_fd, struct sockaddr_in *server_addr, int port);
void communication_handler(int server_fd, struct sockaddr_in *client_addr);
void set_event_listeners(
    void(*once)(char *, int),
    void(*on)(char *, int),
    void(*close)(),
    void(*end)()
);

void handle_once_event();
void handle_on_event();
void handle_close_event();
void handle_end_event();

// private functions
static ssize_t emit(int client_fd, const void *response, size_t response_len, int flag);
static void convert_binary_ip_to_v4(struct sockaddr_in *client_addr, char *buffer, size_t buffer_len);
static void clean_up_zombies(int signo);
static void DEFAULT_ONCE_EVENT(char *data, int bytes);     // once event default function (if user didn't define this)
static void DEFAULT_ON_EVENT(char *data, int bytes);       // on_data event default function (if user didn't define this)
static void DEFAULT_CLOSE_EVENT();        // on_close event default function (if user didn't define this)
static void DEFAULT_END_EVENT();          // on_end event default function (if user didn't define this)
// static void DEFAULT_ERROR_EVENT();   // error event default function (if user didn't define this)
#endif