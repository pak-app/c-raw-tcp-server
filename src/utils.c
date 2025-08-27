#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <arpa/inet.h> // sockaddr_in, inet_addr

static char **ip_storage = {NULL};

void init_black_list_ips(char **black_list_ips_ptr, size_t init_size_capacity) {
    black_list_ips_ptr = (char**) calloc(init_size_capacity, sizeof(char[INET_ADDRSTRLEN]));
    ip_storage = black_list_ips_ptr;
}
int set_ip(char ip[INET_ADDRSTRLEN]);
int remove_ip(char ip[INET_ADDRSTRLEN]);