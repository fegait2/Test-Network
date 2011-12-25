#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#define BACKLOG 10


int main() {
	int client_fd;
    int listen_fd = socket( AF_INET, SOCK_DGRAM, 0 );
    struct sockaddr_in my_addr;
    struct sockaddr_in remote_addr;
    char buf[512] = {0};
    int   rcvbuf = 10000;
    socklen_t   rcvbufsize = 4;  
    int fLen = 0;
    fd_set lfdset;
    socklen_t sin_size = sizeof(struct sockaddr_in); 
    my_addr.sin_family=AF_INET;
    my_addr.sin_port=htons(7880);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listen_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        printf("bind error\n");
    }
/*
    if (listen(listen_fd, BACKLOG) == -1) {
        printf("listen error!\n");
        exit(1);
    }
*/ 
	char buf1[512];
	int n = recvfrom(listen_fd, (void *)buf1, 512, 0 ,(struct sockaddr *)&remote_addr, &sin_size);
	printf("Recive msg: %d-%s\n",n, buf1);
/*
    if ((client_fd = accept(listen_fd, (struct sockaddr *)&remote_addr, &sin_size)) == -1) {
        printf("accept error\n");
        exit(1);
    }

	printf("get a connections\n");

	while(read(client_fd, buf, 512)) {
		printf("read:%s\n", buf);
	}
*/
	return 0;
}
