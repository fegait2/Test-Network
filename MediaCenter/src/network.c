#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "network.h"

int network_init(socket_conf *sconf) {
	sockaddr_in addr;
	int fd = 0;
	if( (sconf->fd = socket(sconf->domain, sconf->type, 0)) == -1 ) {
        printf("Create socket error!\n");
		return -1;
	}

	addr.sin_family=AF_INET;
	addr.sin_port=htons(sconf->port);
	addr.sin_addr.s_addr = INADDR_ANY;
	
	fcntl(sconf->fd, F_SETFD, 1);
	int i = 1;
	setsockopt( sconf->fd, SOL_SOCKET, SO_REUSEADDR, (void*) &i, sizeof(i) );
	if( bind(sconf->fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1 ) {
		printf("Bind addr error!!\n");
		return -1;
	}

	if ( listen(sconf->fd, sconf->max_connection) == -1 ) {
        printf("Listen port error!\n");
        return -1;
    }
	fd = sconf->fd;
	return fd;
}

void dispacher(int fd) {
	
	int pid;
	if( (pid = fork()) < 1 ) {
		return;
	} else {
		
	}
}

int network_accept(int fd) {
	sockaddr_in addr;
	int client_fd = -1;
	socklen_t sin_size = sizeof(struct sockaddr_in);
	//while( 1 ) {
	int rc = accept(fd, (struct sockaddr *)&addr, &sin_size);
	printf("accept a new connection\n");
	return rc;
	/*
		if( (client_fd = accept(fd, (struct sockaddr *)&addr, &sin_size)) == -1 ) {
			printf("Accept connection error!!\n");
			return;
		}
	*/
	//	dispacher(client_fd);
		/*
		char buf[512] = {0};
		read(client_fd, buf, 512);
		printf("%s\n", buf);
		close(client_fd);
		*/
	//}
}
