#ifndef _CONF_H_
#define _CONF_H_
typedef struct conf conf;

typedef struct socket_conf {
    int domain;
    int type;
    //sockaddr_in addr;
    int port;
    int max_connection;
    int fd; //listen fd;
} socket_conf;
#endif
