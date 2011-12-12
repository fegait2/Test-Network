#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "mod_core.h"
#include "util.h"

int fd;
int conn_fd;

void http_init();
void http_start();
void http_release();
void process_request(int fd);
int check_request(req_info *header);
void send_respone_header(int status);
void send_respone_body(req_info *header);
void send_message(char *buf, int len);

void create_http(net_module *module) {
	if( !module )
		return;
	//memset(module, 0, sizeof(net_module));
	module->module_init = http_init;
	module->module_start = http_start;
	module->module_release = http_release;
}

void http_init(conf *_conf) {
	//module->process_request = process_request;
	/*
	socket_conf conf;
    conf.domain = AF_INET;
    conf.type = SOCK_STREAM;
    conf.port = 1234;
    conf.max_connection = 10;
	*/
	signal(SIGCLD, SIG_IGN);
    fd = network_init((socket_conf *)_conf);
	int pid = 0;
	//if( (pid = fork()) < 1 )
	//	return;
	//process_request(fd);
}

void http_start() {
	printf("Http start: %d!!\n", getpid());
	while( 1 ) {
		printf("accept pid: %d\n", getpid());
		conn_fd = network_accept(fd);
		int pid = 0;
		if( (pid = fork()) > 0 ) {
			close(conn_fd);
        	continue;
		}
		//dup2(tmpfd, conn_fd);
		//close(tmpfd);
		printf("start child: %d\n", getpid());
    	process_request(conn_fd);
		break;
		//exit(0);
	}
}

void http_release() {
}

void process_request(int fd) {
	char buf[1024] = {0};
	fd_set readset;
	FD_ZERO( &readset );
    FD_SET( fd, &readset );
	int size = 0;
	while( 1 ) {
		if( select(conn_fd + 1, &readset, (fd_set*) 0, (fd_set*) 0, (struct timeval*) 0) < 0 ) 
			break;
        if( (size = read(conn_fd, buf, 1024)) < 1024 ) 
			break;
		
		//printf("size: %d\n", size);
        //printf("%s\n", buf);
        //close(fd);
	}
	
	req_info *header = parse_http(buf);
	/*
	if( header ) {
		printf("size: %d\n", header->size);
		printf("method: %d\n", header->method);
		printf("file: %s\n", header->file);
		printf("protocol: %d\n", header->protocol);
		printf("version: %f\n", header->version);
	}
	*/
	if( !header ) 
		return;

	/*
	char *file = header->file;
	char *query = strchr(file, "?");
	if( query ) {
		*query = '\0';
		query ++;
	}
	*/

	if( check_request(header) < 0 ) {
		return;
	}

	send_respone_line(0);
	send_respone_body(header);
/*
	write(fd, "HTTP/1.0 ", 8);
	write(fd, 200, 4);
	write(fd, "OK", 2);
	write(fd, "Content-Type:text/html\r\n",24);
	write(fd, "\r\n\r\n", 4);
	write(fd, "<html><body>test</body></html>", 30);
*/
	release_header(header);
	close(conn_fd);
}

int check_request(req_info *header) {
	if( !header || !header->_req_line->path )
        return -1;
    char *file = header->_req_line->path;
    if( *file == '/' ) {
        file ++;
    }

    if( access(file, 4) < 0 ) { //检查读权限
        printf("The file is not exist\n");
        return -2;
    }

	return 0;
}

void send_respone_line(int status) {
	int len = 8 + 4 + 4;
	char buf[len] ;
	int tmp = 200;
	memcpy(buf, "HTTP/1.0 ", 8);
	memcpy(buf + 8, &tmp, 4);
	memcpy(buf + 8 + 4, " OK\r\n", 4);
	//memcpy(buf + 16, "Content-Type:text/html\r\n\r\n", 26);
	send_message(buf, len);
	/*
	write(conn_fd, "HTTP/1.0 ", 8);
    write(conn_fd, 200, 4);
    write(conn_fd, "OK", 2);
    write(conn_fd, "Content-Type:text/html\r\n",24);
    write(conn_fd, "\r\n\r\n", 4);
	*/
}

void send_response_header(req_info *info) {
	send_message("Content-Type:text/html\r\n\r\n", 26);
}
/*
void parse_file_type(char *file) {
}
*/
void send_respone_cgi_data(char *file) {
	int pid = -1;
	int p_fd[2];
	if( pipe(p_fd) < 0 ) {
		printf("Create pipe error!\n");
		return;
	}

	if( (pid = fork()) > 0 ) {//pararent
		close(p_fd[1]);
		char buf[512];
		int len = 0;
		printf("read before\n");
		while( (len = read(p_fd[0], buf, 512)) ) {
			//read(p_fd[0], buf, 512)
			printf("read len: %d\n", len);
			if( len > 0 ) {
				printf("%s\n", buf);
				send_message(buf, len);
			}
		}
		printf("len: %d\n", len);
		close(p_fd[0]);
		return;
	} else {
		close(p_fd[0]);
		dup2(p_fd[1], STDOUT_FILENO);
		int rc = execlp("./1.cgi", file, NULL);
		printf("Exec the CGI error!\n");
		//close(p_fd[1]);
	}
}

void send_respone_file_data(char *file) {
	char buf[512];
	int file_fd;
	if( (file_fd = open(file, O_RDONLY)) < 0 ) {
        printf("Open file error!!\n");
        return;
    }

    int r_size = 0;
    while( (r_size = read(file_fd, buf, 512)) > 0 ) {
        send_message(buf, r_size);
    }
	close(file_fd);
}

void make_envp(req_info *header) {
	//子线程即cgi的线程会自动继承该环境变量
	if( header->_req_line->query )
		setenv("QUERY_STRING", header->_req_line->query, 1);
}

void send_respone_body(req_info *header) {

	char *file = header->_req_line->path;
    if( *file == '/' ) {
        file ++;
    }

	int f_len = strlen(file);
	if( strncmp(file + f_len - 4, ".cgi", 4) == 0 ) {
		make_envp(header);
		send_respone_cgi_data(file);
	} else {
		printf("file: %s\n", file);
		send_response_header(header);
		send_respone_file_data(file);
	}
}

void send_message(char *buf, int len) {
	if( buf ) {
		write(conn_fd, buf, len);
	}
}
