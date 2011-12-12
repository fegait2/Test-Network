#include "conf.h"

typedef struct net_module {
	void(*module_init)(conf *_conf);
	void(*module_start)();
	void(*module_release)();
} net_module;

void register_all_modules();
net_module* register_module(int port);
void modules_init(conf *_conf);
void modules_start();
//void dipatche(int fd);
