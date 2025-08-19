
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>    // close()
#include <arpa/inet.h> // sockaddr_in, inet_addr


void server(char *address, int port, int bufSize)
{

    if (address == NULL)
        address = "localhost";
    if (&port == NULL)
        port = 8080;
    if (&bufSize == NULL)
        bufSize = 1024;

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[bufSize];
    pid_t pid_number;

    socklen_t addr_len = sizeof(client_addr);

    // 1. Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind socket to IP/port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // any local IP
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
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

    // Handle the clients connect.

    
    while (1)
    {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        
        if (client_fd < 0)
        {
            perror("accept failed");
            close(client_fd); // close client socket
        }

        pid_number = fork();
        
        if (pid_number < 0)
        {
            perror("Fork operation failed.");
            close(server_fd);
            return;
        }

        if (pid_number == 0) {

            // Convert IP to string
            char client_ip_v4[INET_ADDRSTRLEN]; // buffer for IPv4 string
            inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip_v4, INET_ADDRSTRLEN);
            
    
            printf("Client %s connected!\n", client_ip_v4); // Log connected client

            while (1)
            {
                // recieve number of bytes of data we get from cleint
                // if the bytes variable is 0 it means the client disconnected
                // if it is more than 0, it received a data
                int bytes = recv(client_fd, buffer, bufSize - 1, 0);
                
                // handler cleint disconnection
                if (bytes <= 0)
                {
                    close(client_fd);
                    printf("Client disconnected %s", client_ip_v4);
                    break;
                }

                char *reply = "Hello from server\n";    // response message
                ssize_t send_status = send(client_fd, reply, strlen(reply), 0);
                if ( send_status < 0)
                {
                    printf("Send operation failed and disconnect cleint %d", client_fd);
                    close(client_fd);
                }
                buffer[bytes] = '\0';
                printf("Received: %s\n", buffer);
            }
        }
    }

    close(server_fd);
}