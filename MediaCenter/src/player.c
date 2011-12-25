#include "player.h"
#include "tcp_player.h"

NPlayer *create_player(PlayerType type) {
	NPlayer *player = NULL;
	switch( type ) {
	case PLAYER_TYPE_TCP:
		player = create_tcp_player();
		break;
	}
	return player;
}

void release_player(NPlayer *player) {
	if( !player )
		return;
	player->stop();
	free(player);
}
