#include <stdio.h>
#include "tcp.h"

void on(char *data, int bytes)
{
    data[bytes] = '\0';
    printf("[ON EVENT]: Received: %s\n", data);
}

void once(char *data, int bytes)
{
    data[bytes] = '\0';
    printf("[ONCE EVENT]: Received: %s\n", data);
}

void on_close(void)
{
    printf("[CLOSE EVENT]: client disconnected\n");
}

void end(void)
{
}


typedef struct
{
    /* data */
    int i;
    char h;
} Test;


int main(void)
{
    Socket* my_socket = server(1024);
    start_server(my_socket, "localhost", 8080);

    return 0;
}
