#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> // sockaddr_in, inet_addr
#include <signal.h>    // signal
#include <sys/wait.h>
#include "tcp.h"

#define DEFAULT_BUF_SIZE 1024    // default buffer size
#define DEFAULT_PORT 8080        // default port number
#define DEFAULT_HOST "localhost" // default host name

int buff_size; // buffer size as global variable

Socket client_socket;           // socket information (connected client ip)

static char **black_list_ips = {NULL}; // setup balcklist IPs to null
static void DEFAULT_ONCE_EVENT(Socket*, char __attribute__((unused)) *data, int __attribute__((unused)) bytes) {} // once event default function (if user didn't define this)
static void DEFAULT_ON_EVENT(Socket*, char __attribute__((unused)) *data, int __attribute__((unused)) bytes) {}   // on_data event default function (if user didn't define this)
static void DEFAULT_CLOSE_EVENT(Socket*) {}                     // on_close event default function (if user didn't define this)
static void DEFAULT_END_EVENT(Socket*) {}                       // end event default function (if user didn't define this)

static ssize_t emit(const void *response, int flag)
{
    int len = strlen(response);
    ssize_t status = send(client_socket.client_fd, response, len, flag);
    if (status < 0)
    {
        perror("emit function failed...");
        exit(1);
    }

    return status;
}

static void end(void) {
    client_socket.events.on_end(&client_socket);    // on end event called (send last messages)
    shutdown(client_socket.client_fd, SHUT_WR);     // graceful shutdown (send fin packet client by kernel)
    close(client_socket.client_fd);                 // close connection
    client_socket.events.on_close(&client_socket);  // call on close event to release resources
    exit(0);
}

static void destroy(void) {
    close(client_socket.client_fd);                 // close connection
    client_socket.events.on_close(&client_socket);  // call on close event to release resources
    exit(0);
}

void bind_server(int server_fd, struct sockaddr_in *server_addr, int port)
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

static void clean_up_zombies(int __attribute__((unused)) signo)
{
    int status;
    pid_t pid;

    // Keep reaping until no more finished children
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        // Use write() instead of printf() in signal handler
        char buf[100];
        // int len = snprintf(buf, sizeof(buf), "Reaped child PID=%d\n", pid);
        write(STDOUT_FILENO, buf, 0);
    }
}

static void convert_binary_ip_to_v4(struct sockaddr_in *client_addr, char *buffer, size_t buffer_len)
{
    inet_ntop(AF_INET, &(client_addr->sin_addr), buffer, buffer_len);
}

void communication_handler(int server_fd, struct sockaddr_in *client_addr)
{

    socklen_t addr_len = sizeof(*client_addr);
    pid_t pid_num;
    int client_fd;
    char buffer[buff_size];

    client_fd = accept(server_fd, (struct sockaddr *)client_addr, &addr_len);

    if (client_fd < 0)
    {
        perror("accept failed");
        return;
    }

    pid_num = fork();

    if (pid_num < 0)
    {
        perror("Fork operation failed.");
        exit(1);
    }

    if (pid_num == 0)
    {
        client_socket.client_fd = client_fd; // set client file descriptor for client socket
        close(server_fd); // in child process there is no need to listening
        // Convert IP to string
        convert_binary_ip_to_v4(client_addr, client_socket.remote_host, sizeof(client_socket.remote_host));
        printf("Size of IP address: %s %lu\n", client_socket.remote_host, sizeof(client_socket.remote_host));
        // once event, get the first message from client
        // This is before the communication loop
        // where a client connected and send the first message
        // 1 ==> the client connected for the first time and send a first message(this usage is for auth, caching, and initialization)
        // 0 ==> the client connected for the
        int once_event = 1;

        while (1)
        {
            // recieve number of bytes of data we get from cleint
            // if the bytes variable is 0 it means the client disconnected
            // if it is more than 0, it received a data
            int bytes = recv(client_fd, buffer, buff_size - 1, 0);

            // handler cleint disconnection
            // it could set a timer to close connection
            if (bytes <= 0)
            {
                close(client_fd);
                // this is end event
                // client disconnected and should call end
                // then close event.
                // the close event is for releasing resources
                client_socket.end();    // greceful end event
            }

            if (once_event)
            {
                client_socket.events.once(&client_socket, buffer, bytes);
                once_event = 0; // set once event to false because we got the first message and connection
            }
            else
            {
                // On event can implement and call here
                // it is called and gets incoming data from user
                client_socket.events.on_data(&client_socket, buffer, bytes);
            }
        }
        exit(0);
    }
    else
    {
        close(client_fd); // in parent process there is no need to listen for client
    }
}

Socket *server(int buffer_size)
{
    if (buffer_size == 0)
        buffer_size = DEFAULT_BUF_SIZE;

    buff_size = buffer_size; // set buffer size
    // Set defaults for events functions
    client_socket.events.on_close = DEFAULT_CLOSE_EVENT;
    client_socket.events.on_end = DEFAULT_END_EVENT;
    client_socket.events.on_data = DEFAULT_ON_EVENT;
    client_socket.events.once = DEFAULT_ONCE_EVENT;

    client_socket.emit = emit; // set the send data function
    client_socket.end = end;    // set the end connection function
    client_socket.destroy = destroy; // set the destroy connection function
    // Setup child end signal handler
    signal(SIGCHLD, clean_up_zombies); // set signal to clean up zombies process

    return &client_socket;
}

void start_server(Socket *defined_socket, const char *address, int port)
{

    if (address == NULL)
        address = DEFAULT_HOST;
    if (port == 0)
        port = DEFAULT_PORT;

    client_socket = *defined_socket;

    int server_fd;
    struct sockaddr_in server_addr, client_addr; // server and client addr contains clients information

    // Init server
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Handle server binding
    bind_server(server_fd, &server_addr, port);

    while (1)
    {
        communication_handler(server_fd, &client_addr);
    }

    close(server_fd);
    return;
}
