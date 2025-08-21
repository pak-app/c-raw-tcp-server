#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>      // sockaddr_in, inet_addr
#include <signal.h>         // signal
#include <sys/wait.h>
#include "tcp.h"

#define DEFAULT_BUF_SIZE 1024       // default buffer size
#define DEFAULT_PORT 8080           // default port number
#define DEFAULT_HOST "localhost"    // default host name

Socket client_socket;   // socket information (connected client ip)
EventListeners event_listeners; // Event listeners function


static void DEFAULT_ONCE_EVENT(char *data){};     // once event default function (if user didn't define this)
static void DEFAULT_ON_EVENT(char *data){};       // on event default function (if user didn't define this)
static void DEFAULT_CLOSE_EVENT(){};        // close event default function (if user didn't define this)
static void DEFAULT_END_EVENT(){};          // end event default function (if user didn't define this)


void bind_listen_server(int server_fd, struct sockaddr_in *server_addr, int port)
{
    // 1. Create socket
    if (server_fd < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind socket to IP/port
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY; // any local IP
    server_addr->sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0)
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 3. Listen
    if (listen(server_fd, 5) < 0)
    {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", port);
}

static void clean_up_zombies(int signo) {
    int status;
    pid_t pid;

    // Keep reaping until no more finished children
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Use write() instead of printf() in signal handler
        char buf[100];
        int len = snprintf(buf, sizeof(buf), "Reaped child PID=%d\n", pid);
        write(STDOUT_FILENO, buf, len);
    }
}

static void convert_binary_ip_to_v4(struct sockaddr_in *client_addr, char *buffer, size_t buffer_len) {
    inet_ntop(AF_INET, &(client_addr->sin_addr), buffer, buffer_len);
}

void client_handler(int server_fd, int client_fd, struct sockaddr_in *client_addr, int buff_size)
{

    socklen_t addr_len = sizeof(*client_addr);
    pid_t pid_num;
    char buffer[buff_size];

    client_fd = accept(server_fd, (struct sockaddr *)client_addr, &addr_len);

    if (client_fd < 0)
    {
        perror("accept failed");
    }

    pid_num = fork();

    if (pid_num < 0)
    {
        perror("Fork operation failed.");
        return;
    }

    if (pid_num == 0)
    {

        // Convert IP to string
        
        // socket.remote_host = "localhost";
        convert_binary_ip_to_v4(client_addr, client_socket.remote_host, sizeof(client_socket.remote_host));
        
        close(server_fd);   // in child process there is no need to listening

        printf("Client %s connected!\n", client_socket.remote_host); // Log connected client

        while (1)
        {
            // recieve number of bytes of data we get from cleint
            // if the bytes variable is 0 it means the client disconnected
            // if it is more than 0, it received a data
            int bytes = recv(client_fd, buffer, buff_size - 1, 0);

            // handler cleint disconnection
            if (bytes <= 0)
            {
                close(client_fd);
                printf("Client disconnected %s", client_socket.remote_host);
                // break;
                exit(1);    // exit
            }

            char *reply = "Hello from server\n"; // response message
            ssize_t send_status = send(client_fd, reply, strlen(reply), 0);
            if (send_status < 0)
            {
                printf("Send operation failed and disconnect cleint %d", client_fd);
                close(client_fd);
                // break;
                exit(1);
            }
            buffer[bytes] = '\0';
            printf("Received: %s\n", buffer);
        }
        exit(0);
    }
    else {
        close(client_fd);   // in parent process there is no need to listen for client
    }
}

void set_event_listeners(
    void(*once)(char *),
    void(*on)(char *),
    void(*close)(),
    void(*end)()
) {
    event_listeners.once = once;
    event_listeners.on = on;
    event_listeners.end = end;
    event_listeners.close = close;
}

void server(const char *address, int port, int buff_size)
{

    if (address == NULL)
        address = DEFAULT_HOST;
    if (port == 0)
        port = DEFAULT_PORT;
    if (buff_size == 0)
        buff_size = DEFAULT_BUF_SIZE;

    event_listeners.close = DEFAULT_CLOSE_EVENT;
    event_listeners.end = DEFAULT_END_EVENT;
    event_listeners.on = DEFAULT_ON_EVENT;
    event_listeners.once = DEFAULT_ONCE_EVENT;

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;

    // Setup child end signal handler
    signal(SIGCHLD, clean_up_zombies);

    // Init server
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Handle bind and listen server
    bind_listen_server(server_fd, &server_addr, port);

    // Main cleint connection and communication loop
    while (1)
    {
        client_handler(server_fd, client_fd, &client_addr, buff_size);
    }

    close(server_fd);
}