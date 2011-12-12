#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "mod_core.h"
#include "util.h"
#include "mod_rtsp.h"

static int fd;
static int conn_fd;

void rtsp_init();
void rtsp_start();
void rtsp_release();

static void process_request();
static void process_command(req_info *header);
static int check_request(req_info *header);
static void send_respone_line(int status);
static void send_respone_header(int status);
static void send_respone_body(req_info *header);
static void send_message(char *buf, int len);
char *get_string_int_line(char*, char, int);
void send_options_response(req_info *req);
void send_discribe_reponse(req_info *req);

void create_rtsp(net_module *module) {
	if( !module )
		return;
	//memset(module, 0, sizeof(net_module));
	module->module_init = rtsp_init;
	module->module_start = rtsp_start;
	module->module_release = rtsp_release;
}

void rtsp_init(conf *_conf) {
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
	//if( (pid = fork()) < 1 )
	//	return;
	//process_request(fd);
}

void rtsp_start() {
	printf("rtsp start: %d!!\n", getpid());
	while( 1 ) {
		conn_fd = network_accept(fd);
		int pid = 0;
		if( (pid = fork()) > 0 ) {
			close(conn_fd);
        	continue;
		}
		close(fd);
		//int r  =1;
		//(void) setsockopt(conn_fd, IPPROTO_TCP, TCP_NOPUSH, (void*) &r, sizeof(r) );
    	process_request();
		exit(0);
		break;
	}
}

void rtsp_release() {
}

static void process_request() {
	char buf[1024] = {0};
	fd_set readset;
	int size = 0;
	int t_size = 0;
	int ts = 0;
	req_info *header = NULL;
	while( 1 ) {
		//if( (ts = select(conn_fd + 1, &readset, 0, 0, (struct timeval*) 0)) < 0 ) 
		//	break;
		//printf("ts: %d\n", ts);
        if( (size = read(conn_fd, buf + t_size, 1024)) <= 0 ) {
			printf("if size:%d, %d\n", size, getpid());
			break;
		}
		printf("size: %d, %d\n", size, getpid());
		t_size += size;
		
		if( t_size > 3 && buf[t_size - 4] == 13 && buf[t_size - 3] == 10 && buf[t_size - 2] == 13 && buf[t_size - 1] == 10 ) {
			printf("rtsp request:%s\n", buf);
			header = parse_http(buf);
			if( header ) {
				process_command(header);
				release_header(header);
				header = NULL;
			}
			t_size = 0;	
			size = 0;
			memset(buf, 0 , 1024);
			//break;
		}
		
	}


	/*
	if( header ) {
		printf("size: %d\n", header->size);
		printf("method: %d\n", header->method);
		printf("file: %s\n", header->file);
		printf("protocol: %d\n", header->protocol);
		printf("version: %f\n", header->version);
	}
	*/

	/*
	char *file = header->file;
	char *query = strchr(file, "?");
	if( query ) {
		*query = '\0';
		query ++;
	}
	*/
/*
	if( check_request(header) < 0 ) {
		return;
	}

	send_respone_line(0);
	send_respone_body(header);
*/
/*
	write(fd, "HTTP/1.0 ", 8);
	write(fd, 200, 4);
	write(fd, "OK", 2);
	write(fd, "Content-Type:text/html\r\n",24);
	write(fd, "\r\n\r\n", 4);
	write(fd, "<html><body>test</body></html>", 30);
*/
	close(conn_fd);
	//if( header->_req_line->method == OPTIONS )
	//shutdown(conn_fd, SHUT_WR);
}

static int check_request(req_info *header) {
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

static void send_respone_line(int status) {
	int len = 8 + 4 + 4;
	char buf[len] ;
	int tmp = 200;
	memcpy(buf, "RTSP/1.0 ", 9);
	memcpy(buf + 9, "200", 3);
	memcpy(buf + 9 + 3, " OK\r\n", 5); 
	
	//memcpy(buf + 16, "Content-Type:text/html\r\n\r\n", 26);
	send_message("RTSP/1.0 200 OK\r\n", strlen("RTSP/1.0 200 OK\r\n"));
	/*
	write(conn_fd, "HTTP/1.0 ", 8);
    write(conn_fd, 200, 4);
    write(conn_fd, "OK", 2);
    write(conn_fd, "Content-Type:text/html\r\n",24);
    write(conn_fd, "\r\n\r\n", 4);
	*/
}

static void send_response_header(req_info *info) {
	send_message("Content-Type:text/html\r\n\r\n", 26);
}
/*
void parse_file_type(char *file) {
}
*/

static void send_respone_file_data(char *file) {
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

static void make_envp(req_info *header) {
}

static void process_command(req_info *header) {
	//char a = '\r';
	//char b = '\n';
	switch(header->_req_line->method) {
	case CREATE:
		break;
	case OPTIONS:
		send_respone_line(0);
		send_options_response(header);
		break;
	case DESCRIBE: 
		send_respone_line(0);
		send_discribe_reponse(header);
		break;
	case SETUP:
		
		printf("Recive Setup command!!!!!!!\n");
	}
}

void send_options_response(req_info *req) {
		char *cseq = get_string_int_line("CSeq", ':', ((req_rtsp_extend_header *)(req->_req_header->extend_header))->c_seq);
		write(conn_fd, cseq, strlen(cseq));
		free(cseq);
		write(conn_fd, "Public: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n", 
				strlen("Public: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n"));
}

char *build_sdp_content() {
	char *sdp = (char *)calloc(sizeof(char), 400);
	char sid[20]= {0};
	struct timeval t_us;
	strcpy(sdp, "v=0\r\n");
	//strcat(sdp, "o=- ");
   	//gettimeofday(&t_us, NULL); 
	//sprintf(sid, "%ld ", t_us.tv_sec*1000 + t_us.tv_usec);
	//strcat(sdp, sid);
	//gettimeofday(&t_us, NULL);
	//sprintf(sid, "%ld ", t_us.tv_sec*1000 + t_us.tv_usec);
	//strcat(sdp, sid);
	strcat(sdp, "o=- 1323590466063534 1 IN IP4 127.0.0.1\r\n");
	strcat(sdp, "s=MPEG-4 Video, streamed by the LIVE555 Media Server\r\n");
	//strcat(sdp, "i=1.m4e\r\n");
	strcat(sdp, "t=0 0\r\n");
	strcat(sdp, "a=tool:LIVE555 Streaming Media v2011.12.02\r\n");
	strcat(sdp, "a=type:broadcast\r\n");
	strcat(sdp, "a=control:*\r\n");
	strcat(sdp, "a=range:npt=0-\r\n");
	strcat(sdp, "a=x-qt-text-nam:MPEG-4 Video, streamed by the LIVE555 Media Server\r\n");
	strcat(sdp, "a=x-qt-text-inf:1.m4e\r\n");
	strcat(sdp, "m=video 0 RTP/AVP 96\r\n");
	strcat(sdp, "c=IN IP4 0.0.0.0\r\n");
	strcat(sdp, "b=AS:500\r\n");
	strcat(sdp, "a=rtpmap:96 MP4V-ES/90000\r\n");
	strcat(sdp, "a=control:track1\r\n");
	//printf("sdp: %s\n", sdp);
	return sdp;
/*
	char *tmp1 = "v=0\r\n\
o=- 2890844256 2890842807 IN IP4 127.0.0.1\r\n\
s=I came from a web page\r\n\
t=0 0\r\n\
c=IN IP4 0.0.0.0\r\n\
m=video 0 RTP/AVP 31\r\n\
a=control:video\r\n\
m=audio 0 RTP/AVP 3\r\n\
a=control:audio\r\n\r\n";
	return tmp1;
*/
}

void send_discribe_reponse(req_info *req) {
	char *sdp = build_sdp_content();
	if( !sdp )
		return;
	
	char *cseq = get_string_int_line("CSeq", ':', ((req_rtsp_extend_header *)(req->_req_header->extend_header))->c_seq);
	
	int sdp_len = strlen(sdp);
	printf("cseq: %s\n", cseq);
	write(conn_fd, cseq, strlen(cseq));
	free(cseq);
	char *cl = get_string_int_line("Content-Length", ':', sdp_len);
	//printf("con len: %s\n", cl);
	write(conn_fd, "Date: Mon, Dec 12 2011 12:51:50 GMT\r\n", strlen("Date: Mon, Dec 12 2011 12:51:50 GMT\r\n"));
	write(conn_fd, "Content-Base: rtsp://127.0.0.1:554/1.3gp/\r\n", strlen("Content-Base: rtsp://127.0.0.1:554/1.3gp/\r\n"));
	write(conn_fd, cl, strlen(cl));
	free(cl);
	write(conn_fd, "Content-Type:application/sdp\r\n", strlen("Content-Type:application/sdp\r\n"));
	write(conn_fd, "\r\n", 2);
	write(conn_fd, sdp, sdp_len);
	write(conn_fd, "\r\n", 2);
	//free(sdp);
	printf("write end: %d\n", getpid());
	fflush(stdout);
	
	/*
	char tmp[1000] = {0};
	char *cl = get_string_int_line("Content-Length", ':', strlen(sdp));
	strcpy(tmp, "RTSP/1.0 200 OK\r\n");
	strcat(tmp, cseq);
	strcat(tmp, cl);
	strcat(tmp, "Content-Type:application/sdp\r\n\r\n");
	strcat(tmp, sdp);
	printf("content: %s\n", tmp);
	fflush(stdout);
	write(conn_fd, tmp, strlen(tmp));
	*/
}

static void send_respone_body(req_info *header) {

	char *file = header->_req_line->path;
    if( *file == '/' ) {
        file ++;
    }

	int f_len = strlen(file);
}

static void send_message(char *buf, int len) {
	if( buf ) {
		write(conn_fd, buf, len);
	}
}

char *get_string_int_line(char *name, char separator, int value) {
	if( !name )
		return NULL;
	int name_len = strlen(name);
	char *buf = (char *) calloc(sizeof(char), strlen(name) + 1 + 5 + 2); //separator, int value and \r\n
	if( !buf )
		return NULL;
	strcpy(buf, name);
	buf[name_len] = separator;
	sprintf(buf + name_len + sizeof(char), "%d", value);
	strcat(buf, "\r\n");
	return buf;
}

