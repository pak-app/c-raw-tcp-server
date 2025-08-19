#include <stdio.h>
#include "tcp.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // close()
#include <arpa/inet.h>      // sockaddr_in, inet_addr

// Child process routine:
void	child_routine(pid_t pid, int *nb)
{
	printf("Child: Hi! I'm the child. PID received from fork = %d\n", pid);
	printf("Child: The number is %d\n", *nb);
}

// Parent process routine:
void	parent_routine(pid_t pid, int *nb)
{
	printf("Parent: I'm the parent. PID received from fork = %d\n", pid);
	printf("Parent: The number is %d\n", *nb);
	*nb *= 2;
	printf("Parent: The modified number is %d\n", *nb);
}

int main()
{

    tcpServer("localhost", 8080, 1024);

	return 0;
}
