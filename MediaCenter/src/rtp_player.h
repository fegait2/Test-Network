#include "player.h"
typedef enum RTPType {
	UDP,
	TCP
} RTPType;

typedef struct RTPPlayerConf {
	int rtpPort;
	int rtcpPort;
  char *ip[20];
	RTPType type;
} RTPPlayerConf;

NPlayer* create_tcp_player(RTPPlayerConf *conf);
