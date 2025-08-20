#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>      // sockaddr_in, inet_addr

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

const char* convert_binary_ip_to_v4(struct sockaddr_in *client_addr) {
    char ip_v4_string[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr->sin_addr), ip_v4_string, INET_ADDRSTRLEN);
    return ip_v4_string;
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
        close(client_fd); // close client socket
    }

    pid_num = fork();

    if (pid_num < 0)
    {
        perror("Fork operation failed.");
        close(server_fd);
        return;
    }

    if (pid_num == 0)
    {

        // Convert IP to string
        const char client_ip_v4[] = convert_binary_ip_to_v4(client_addr); // buffer for IPv4 string

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
            }
            buffer[bytes] = '\0';
            printf("Received: %s\n", buffer);
        }
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