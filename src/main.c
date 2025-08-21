#include <stdio.h>
#include "tcp.h"

void on(char *data) {

}

void once(char *data) {

}

void close() {

}

void end() {

}

int main()
{
    set_event_listeners(once, on, close, end);
    int server_id = server("localhost", 8080, 1024);
    start_server(server_id);

	return 0;
}
