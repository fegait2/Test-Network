#include <stdio.h>
#include "network.h"

int main() {
	
	socket_conf conf;
/*
	conf.domain = AF_INET;
	conf.type = SOCK_STREAM;
	conf.port = 1234;
	conf.max_connection = 10;
	register_module(1234);
	modules_init(&conf);
*/
	
    conf.domain = AF_INET;
	conf.type = SOCK_STREAM;
	conf.port = 8554;
	conf.max_connection = 2;
	register_module(8554);	
	modules_init(&conf);

	modules_start();
	
	getchar();	
	return 0;
}
