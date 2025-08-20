#include <stdio.h>
#include "tcp.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // close()
#include <arpa/inet.h>      // sockaddr_in, inet_addr
#include <fcntl.h>

int main()
{

    server("localhost", 8080, 1024);

	return 0;
}
