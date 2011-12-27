#include "rtp_player.h"
static int init_tcp_player(char *path);
static void tcp_play();
static void tcp_seek(int position);
static void tcp_pause();
static void tcp_stop();

static RTPPlayerConf *conf = NULL;

NPlayer* create_tcp_player(RTPPlayerConf *_conf) {
	NPlayer *player = (NPlayer *)calloc(1, sizeof(NPlayer));
	player->init_player = init_tcp_player;
	player->play = tcp_play;
	player->seek = tcp_seek;
	player->pause = tcp_pause;
	player->stop = tcp_stop;

	if( !_conf ) {
		conf = (RTPPlayerConf *) calloc(sizeof(char), sizeof(RTPPlayerConf));
		memcpy(conf, _conf, sizeof(RTPPlayerConf));
	}

	return player;
}

int init_tcp_player(char *path) {
	
}

void tcp_play() {
}

void tcp_seek(int position) {
}

void tcp_pause() {
}

void tcp_stop() {
}
