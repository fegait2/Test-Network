#include "player.h"
#include "rtp_player.h"

NPlayer *create_player(PlayerType type) {
	NPlayer *player = NULL;
	switch( type ) {
	case PLAYER_TYPE_RTP_UDP:
	case PLAYER_TYPE_RTP_TCP:
		//player = create_tcp_player(type);
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
