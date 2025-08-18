
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // close()
#include <arpa/inet.h>      // sockaddr_in, inet_addr

void readData(char *fileName)
{
    FILE *fptr;

    fptr = fopen(fileName, "r");

    char buffer[8];
    size_t bytesRead;

    if (fptr == NULL)
    {
        printf("File %s not found", fileName);
        return;
    }

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), fptr)) > 0)
    {
        // Print each byte in hex
        for (size_t i = 0; i < bytesRead; i++)
        {
            printf("%c ", buffer[i]);
        }
        printf("\n");
    }

    fclose(fptr);
}

void readLines(char *fileName)
{
    FILE *fptr;

    fptr = fopen(fileName, "r");

    char buffer[8];
    size_t bytesRead;

    if (fptr == NULL)
    {
        printf("File %s not found", fileName);
        return;
    }

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), fptr)) > 0)
    {
        // Print each byte in hex
        for (size_t i = 0; i < bytesRead; i++)
        {
            if(&buffer[i] == "\n") {
                printf("\n");
            }
            printf("%c", buffer[i]);
        }
    }

    fclose(fptr);
}

void tcpServer(char *address, int port, int bufSize){

    if (address == NULL) address = "localhost";
    if (&port == NULL) port = 8080;
    if (&bufSize == NULL) bufSize = 1024;


    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[bufSize];
    socklen_t addr_len = sizeof(client_addr);

    // 1. Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind socket to IP/port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // any local IP
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 3. Listen
    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

        // 4. Accept clients forever
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("accept failed");
            continue;  // continue accepting next client
        }

        printf("Client connected!\n");

        // 5. Communicate
        int bytes = read(client_fd, buffer, bufSize);
        buffer[bytes] = '\0';
        printf("Received: %s\n", buffer);

        char *reply = "Hello from server\n";
        send(client_fd, reply, strlen(reply), 0);

        close(client_fd);  // close client socket
    }
    close(server_fd);


}