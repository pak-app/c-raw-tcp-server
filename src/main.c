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

void on_close()
{
    printf("[CLOSE EVENT]: client disconnected\n");
}

void end()
{
}


typedef struct
{
    /* data */
    int i;
    char h;
} Test;


int main()
{
    // int server_id = server("localhost", 8080, 1024);
    // set_event_listeners(once, on, on_close, end);
    // start_server(server_id);
    Test t1 = {12, 's'};
    Test t2 = {11, 'w'};
    t2 = t1;
    printf("MY T2 STRUCT:%d %c\n", t2.i, t2.h);
    return 0;
}
