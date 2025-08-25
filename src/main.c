#include <stdio.h>
#include "tcp.h"

void on(Socket* c_socket, char *data, int bytes)
{
    data[bytes] = '\0';
    printf("[ON EVENT]: Received: %s\n", data);
    const char response[] = "Hello client!!!\n";
    c_socket->emit(response, 0);
}

void once(Socket* c_socket, char *data, int bytes)
{
    data[bytes] = '\0';
    printf("[ONCE EVENT]: Received: %s\n", data);
    const char response[] = "Hello client for the first connection!!!\n";
    c_socket->emit(response, 0);
}

void on_close(__attribute__((unused)) Socket* c_socket)
{
    printf("[CLOSE EVENT]: client disconnected\n");
}

void end(__attribute__((unused)) Socket* c_socket)
{
    printf("[END EVENT] end called.\n");
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
    my_socket->events.on_close = on_close;
    my_socket->events.on_data = on;
    my_socket->events.once = once;
    my_socket->events.on_end = end;
    
    start_server(my_socket, "localhost", 8080);

    return 0;
}
