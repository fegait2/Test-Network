#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <process.h>
#include "rtp_win32.h"
#include "rtp_api.h"
#include "sysdep.h"
#include <sys/types.h>
#include <sys/timeb.h>

extern u_long md_32(char *string, int length);
socktype _sys_create_socket(int type) {
	SOCKET skt;

	skt =WSASocket(AF_INET, type, 0,
		(LPWSAPROTOCOL_INFO)NULL, 0,
		WSA_FLAG_MULTIPOINT_C_LEAF | WSA_FLAG_MULTIPOINT_D_LEAF);

	if(skt == INVALID_SOCKET)
		return(_SYS_INVALID_SOCKET);
	else
		return(skt);

}

int _sys_close_socket(socktype skt) {

	int err;
	err = closesocket(skt);
	if(err == SOCKET_ERROR)
		return(_SYS_SOCKET_ERROR);
	else
		return(0);
}

int _sys_connect_socket(socktype skt, struct sockaddr_in *sa) {

	int err;

	err = WSAConnect(skt,
		(struct sockaddr *) sa,
		sizeof(struct sockaddr_in),
		NULL,
		NULL,
		NULL,
		NULL);

	if(err == SOCKET_ERROR)
		return(_SYS_SOCKET_ERROR);
	else
		return(0);
}

int _sys_set_ttl(socktype skt, int ttl) {

	int lttl;
	int cbRet, nRet;

	lttl = ttl;
	nRet = WSAIoctl (skt,             /* socket */
		SIO_MULTICAST_SCOPE,              /* IP Time-To-Live */
		&lttl,                             /* input */
		sizeof (lttl),                 /* size */
		NULL,                             /* output */
		0,                                /* size */
		&cbRet,                               /* bytes returned */
		NULL,                             /* overlapped */
		NULL);                            /* completion routine */

	if(nRet == SOCKET_ERROR)
		return(_SYS_SOCKET_ERROR);
	else
		return(0);
}

int _sys_get_socket_name(socktype skt, struct sockaddr_in *sa) {

	int len;
	int ret;

	len = sizeof(struct sockaddr);
	ret = getsockname(skt, (struct sockaddr *) sa, &len);
	if(ret == SOCKET_ERROR) {
		ret = WSAGetLastError();
		printf("Getsockname error: %d\n",ret);
		return(_SYS_SOCKET_ERROR);
	}
	else
		return(0);
}

int _sys_set_reuseaddr(socktype skt) {
	int err;
	int one;

	one = 1;
    err = setsockopt(skt,
		SOL_SOCKET,
		SO_REUSEADDR,
		(char*) &one,
		sizeof(one));

	if(err < 0)
		return(_SYS_SOCKET_ERROR);
	else return(0);
}

int _sys_set_reuseport(socktype skt) {

	/* not defined in windows */
	return(0);
}

int _sys_bind(socktype skt, struct sockaddr_in *sa) {

	int nRet;

	nRet = bind (skt,
		(struct sockaddr FAR *) sa,
		sizeof(struct sockaddr));

	if(nRet == SOCKET_ERROR) {

		nRet = WSAGetLastError();
		if(nRet == WSAEADDRINUSE)
			return(_SYS_SOCKET_ADDRINUSE);
		else if(nRet == WSAEADDRNOTAVAIL)
			return(_SYS_SOCKET_ADDRNOTAVAIL);
		else return(_SYS_SOCKET_ERROR);
	} else return(0);

}

int _sys_join_mcast_group(socktype rtpskt, struct sockaddr_in *sa) {
	socktype NewSock;

	NewSock = WSAJoinLeaf (rtpskt,       /* socket */
		(PSOCKADDR)sa,               /* multicast address */
		sizeof (struct sockaddr),              /* length of addr struct */
		NULL,                             /* caller data buffer */
		NULL,                             /* callee data buffer */
		NULL,                             /* socket QOS setting */
		NULL,                             /* socket group QOS */
		JL_BOTH);                         /* do both: send and recv */    
	
	if(NewSock == INVALID_SOCKET)
		return(_SYS_SOCKET_ERROR);
	else return(0);
}


int _sys_sendmsg(socktype s, struct msghdr *m, int f) {
	WSABUF pbuf[10];
	int count, res, i;
	

	/* This is a windows implementation of sendmsg */

	if(m->msg_iovlen > 10) {
		fprintf(stderr,"Error: win version only allows 10 iovecs\n");
		exit(-1);
	}
	for(i = 0; i < m->msg_iovlen; i++) {
		pbuf[i].len = m->msg_iov[i].iov_len;
		pbuf[i].buf = m->msg_iov[i].iov_base;
	}

	if(m->msg_name == NULL) {
		/* Socket is connected */

		res = WSASend(s, pbuf, m->msg_iovlen, &count, 0, NULL, NULL);
		if(res == SOCKET_ERROR) {
			return(_SYS_SOCKET_ERROR);
		}
	} else {
		/* Socket is unconnected */

		res = WSASendTo(s,
						pbuf,
						m->msg_iovlen,
						&count,
						0,
						(struct sockaddr *) m->msg_name,
						m->msg_namelen,
						NULL,
						NULL);
		if(res == SOCKET_ERROR) {
			return(_SYS_SOCKET_ERROR);
		}
	}
	return(0);
}

int _sys_send(socktype skt, char *buf, int buflen, int flags) {
	int res;

	res = send(skt, buf, buflen, flags);
	return(res);
}

int _sys_recvfrom(socktype skt, char *buf, int len, int flags, struct sockaddr *from, int *alen) {

	int res;

	res = recvfrom(skt,
		buf,
		len,
		flags,
		from,
		alen);

	return(res);
}

/* Determine if an IPv4 address is multicast.
   Unlike IN_MULTICAST, take a struct in_addr in network byte order. */

int IsMulticast(struct in_addr addr){
	unsigned int haddr;

	haddr = ntohl(addr.s_addr);
	if(((haddr >> 28) & 0xf) == 0xe)
		return(1);
	else
		return(0);
}

double drand48() {
	double rn;
	int irn;

	irn = rand();
	rn = (double) irn / RAND_MAX;
	return(rn);
}

void srand48(long sv) {

	srand((unsigned int) sv);
}
#ifdef _RTP_SEMI_RANDOM

u_int32 random32(int type){
  return rand();
}

#else
u_int32 random32(int type)
{
  struct {
    struct  timeval tv;
    clock_t cpu;
    int     pid;
  } s;
  
  gettimeofday(&s.tv, 0); 
  s.cpu = clock(); 
  s.pid = _getpid();
  
  return md_32((char *)&s, sizeof(s)); 
  
}                               /* random32 */
#endif

void gettimeofday(struct timeval *tp, void *unused) {

	struct _timeb wt;

	_ftime(&wt);
	tp->tv_sec = wt.time;
	tp->tv_usec = wt.millitm * 1000;
	
}
