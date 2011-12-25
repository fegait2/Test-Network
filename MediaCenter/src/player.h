#ifndef _PLAYER_H_
#define _PLAYER_H_
#include <stdlib.h>

typedef enum {
	PLAYER_TYPE_TCP,
	PLAYER_TYPE_LOCAL
} PlayerType;

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
