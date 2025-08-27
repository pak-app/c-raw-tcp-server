#include <stdio.h>
#include <arpa/inet.h> // sockaddr_in, inet_addr
#ifndef UTILS_H
#define UTILS_H

void init_black_list_ips(char **black_list_ips_ptr, size_t init_size_capacity);
int set_ip(char ip[INET_ADDRSTRLEN]);
int remove_ip(char ip[INET_ADDRSTRLEN]);

#endif