#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>      // sockaddr_in, inet_addr
#include <signal.h>         // signal
#include <sys/wait.h>

#define BUF_SIZE 1024       // default buffer size
#define PORT 8080           // default port number
#define HOST "localhost"    // default host name

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
        char client_ip_v4[INET_ADDRSTRLEN]; // buffer for IPv4 string
        convert_binary_ip_to_v4(client_addr, client_ip_v4, sizeof(client_ip_v4));

        close(server_fd);   // in child process there is no need to listening

        printf("Client %s connected!\n", client_ip_v4); // Log connected client

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
                printf("Client disconnected %s", client_ip_v4);
                break;
            }

            char *reply = "Hello from server\n"; // response message
            ssize_t send_status = send(client_fd, reply, strlen(reply), 0);
            if (send_status < 0)
            {
                printf("Send operation failed and disconnect cleint %d", client_fd);
                close(client_fd);
                break;
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

void server(char *address, int port, int buff_size)
{

    if (address == NULL)
        address = HOST;
    if (&port == NULL)
        port = PORT;
    if (&buff_size == NULL)
        buff_size = BUF_SIZE;

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