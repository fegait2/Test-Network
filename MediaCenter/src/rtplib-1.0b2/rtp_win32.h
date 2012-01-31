#ifndef _RTP_WIN32_H
#define _RTP_WIN32_H

#include "winsock2.h"
typedef SOCKET socktype;

typedef struct sockaddr *caddr_t;

typedef struct iovec {
	char *iov_base;
	int iov_len;
};
struct msghdr {
	caddr_t msg_name;
	int msg_namelen;
	struct iovec *msg_iov;
	int msg_iovlen;
	caddr_t msg_control;
	int msg_controllen;
	int msg_flags;
};

#define _SYS_INVALID_SOCKET    -1
#define _SYS_SOCKET_ERROR         -1
#define _SYS_SOCKET_ADDRINUSE     -2
#define _SYS_SOCKET_ADDRNOTAVAIL  -3

socktype _sys_create_socket(int type);
int _sys_close_socket(socktype skt);
int _sys_connect_socket(socktype skt, struct sockaddr_in *sa);
int _sys_set_ttl(socktype skt, int ttl);
int _sys_get_socket_name(socktype skt, struct sockaddr_in *sa);
int _sys_set_reuseaddr(socktype skt);
int _sys_set_reuseport(socktype skt);
int _sys_bind(socktype rtpskt, struct sockaddr_in *sa);
int _sys_join_mcast_group(socktype rtpskt, struct sockaddr_in *sa);
int _sys_sendmsg(socktype s, struct msghdr *m, int f);
int _sys_send(socktype skt, char *buf, int buflen, int flags);
int _sys_recvfrom(socktype skt, char *buf, int len, int flags, struct sockaddr *from, int *alen);


#endif /* _RTP_WIN32_H */
