#include <stdlib.h>
#include "mod_core.h"
#include "mod_http.h"
#include "mod_rtsp.h"
/*
typedef struct net_module {
    (void)(*process_request)(int fd);
}
*/

typedef struct module_node {
	int port;
	net_module *module;
	struct module_node *next;
} module_node;

static module_node *header = NULL, *tail = NULL;

void register_all_modules() {
	//read a config xml file.
	//This file will record the port:module;
}

net_module* register_module(int port) {
	//load shared lib
	module_node *node = (module_node *)calloc(sizeof(char), sizeof(module_node));
	net_module *module = (net_module *)calloc(sizeof(char), sizeof(net_module));
	if( port == 1234 )
		create_http(module);
	else if( port == 8554 )
		create_rtsp(module);

	node->port = port;
	node->module = module;
	node->next = NULL;
	
	if( !header ) {
		header = tail = node;
	} else {
		tail->next = node;
		tail = node;
	}

	return module;
}

void modules_init(conf *_conf) {
	module_node *_header = header;
	while( _header ) {
		if( ((socket_conf *)_conf)->port == _header->port )
			_header->module->module_init(_conf);
		_header = _header->next;
	}
}

void module_init(int port, conf *_conf) {
}

void dipatche(int fd) {
}

void modules_start() {
	module_node *_header = header;
	while( _header ) {
		if( fork() == 0 ) {
			_header->module->module_start();
		}
        _header = _header->next;

	}
}
