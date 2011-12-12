#ifndef _NETWORK_H_
#define _NETWORK_H_
#include <sys/socket.h>
#include "conf.h"
typedef struct sockaddr_in sockaddr_in;

int network_init(socket_conf *sconf);
int network_accept(int fd);
#endif
