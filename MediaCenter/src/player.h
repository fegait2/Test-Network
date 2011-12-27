#ifndef _PLAYER_H_
#define _PLAYER_H_
#include <stdlib.h>

typedef enum {
	PLAYER_TYPE_RTP_UDP,
	PLAYER_TYPE_RTP_TCP,
	PLAYER_TYPE_LOCAL
} PlayerType;

/*

typedef struct extend_player_conf extend_player_conf;

typedef struct player_conf {
	PlayerType type;
	extend_player_conf *exConf;
} player_conf;
*/

//typedef 
//typedef struct rtp_player_conf {
//} rtp_player_conf;

typedef struct player {
	int(* init_player)(char *pah);
	void(* play)();
	void(* seek)(int position);
	void(* pause)();
	void(* stop)();
} NPlayer;

NPlayer *create_player(PlayerType type);
void release_player(NPlayer *player);
#endif
