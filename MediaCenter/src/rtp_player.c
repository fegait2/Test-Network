#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __unix
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/time.h>  /* struct timeval, gettimeofday() */
#include <sys/param.h> /* Might declare MAXHOSTNAMELEN */
#include <netdb.h>     /* Might declare MAXHOSTNAMELEN */
#include <unistd.h>    /* Might declare gethostname() */
#endif
#include "rtplib-1.0b2/rtp_api.h"
#include "rtplib-1.0b2/rtp_highlevel.h"

#include "rtp_player.h"

static int init_rtp_player(char *path);
static void rtp_play();
static void tcp_seek(int position);
static void tcp_pause();
static void tcp_stop();

static RTPPlayerConf *conf = NULL;

static int sock[2];
static int input;
static context cid;

#define MAXHOSTNAMELEN  50

NPlayer* create_tcp_player(RTPPlayerConf *_conf) {
	NPlayer *player = (NPlayer *)calloc(1, sizeof(NPlayer));
	player->init_player = init_rtp_player;
	player->play = rtp_play;
	player->seek = tcp_seek;
	player->pause = tcp_pause;
	player->stop = tcp_stop;

	if( !_conf ) {
		conf = (RTPPlayerConf *) calloc(sizeof(char), sizeof(RTPPlayerConf));
		memcpy(conf, _conf, sizeof(RTPPlayerConf));
	}

	return player;
}

void RTPSchedule(context cid, rtp_opaque_t opaque, struct timeval *tp)
{
  struct evt_queue_elt *elt;

  elt = (struct evt_queue_elt *) malloc(sizeof(struct evt_queue_elt));
  if (elt == NULL)
    return;

  elt->cid = cid;
  elt->event_opaque = opaque;
  elt->event_time = tv2dbl(*tp);
  
  insert_in_evt_queue(elt);

  return;
}

int init_rtp_lib() {
  rtperror err;
  u_int16 port;
  unsigned char ttl = 1;
  socktype sockt;
  int nfds = 0;
  char *username;
  char hostname[MAXHOSTNAMELEN];
  char cname[MAXHOSTNAMELEN+32];
  struct timeval tp;
  person uid, conid;

  err = RTPCreate(&cid);
  if (err != RTP_OK) {
    printf("%s\n", RTPStrError(err));
    return -1;
  }

  /* Set the RTP Session's host address to send to. */
  err = RTPSessionAddSendAddr(cid,conf->ip, conf->rtpPort, ttl);
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }

  /* Set the RTP Session's local receive address */
  /* We set this to the dest for the following reason:
   *      - It's necessary if the dest is a multicast group, so we join the group
   *           - It's okay if the dest is unicast, since setting a dest which isn't a
   *                  local address is ignored. */
  //err = RTPSessionSetReceiveAddr(cid, remote, port);


  /* Set up our CNAME (canonical name): should be of the form user@host */
  username = getenv("USER");
  if (gethostname(hostname, MAXHOSTNAMELEN) < 0) {
    perror("gethostname");
    return -1;
  }
  if (username) { sprintf(cname, "%s@%s", username, hostname); }
  else { strcpy(cname, hostname); }

  /* Member 0 is the local member (us) */
  err = RTPMemberInfoSetSDES(cid, 0, RTP_MI_CNAME, cname);
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }

  /* Set up our NAME (standard display name) */
  err = RTPMemberInfoSetSDES(cid, 0, RTP_MI_NAME, "RTP Example Server");
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }

  /* Add a single contributor */

  gettimeofday(&tp, NULL);
  srand(tp.tv_usec);
  conid = rand();
  err = RTPSessionAddToContributorList(cid, (int) conid);
  if (err) {
    fprintf(stderr, "%s %s\n", RTPStrError(err), RTPDebugStr());
    return -1;
  }
  err = RTPSessionGetUniqueIDForCSRC(cid,conid,&uid);
  if (err) {
    fprintf(stderr, "%s %s\n", RTPStrError(err), RTPDebugStr());
    return -1;
  }
  sprintf(cname, "Contributor %d", (int) conid);
  err = RTPMemberInfoSetSDES(cid, uid, RTP_MI_CNAME, cname);
  if (err) {
    fprintf(stderr, "%s %s\n", RTPStrError(err), RTPDebugStr());
    return -1;
  }
  err = RTPMemberInfoSetSDES(cid, uid, RTP_MI_NAME, cname);
  if (err) {
    fprintf(stderr, "%s %s\n", RTPStrError(err), RTPDebugStr());
    return -1;
  }


  /* Open the connection.  We're now live on the network. */
  err = RTPOpenConnection(cid);
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }

  /* Get the socket that RTP data is transmitted on, so we can select() on
   *    * it. */
  /* Note that the value passed to it is a socktype *, not an int *. */
  err = RTPSessionGetRTPSocket(cid, &sockt);
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }
  sock[0] = sockt;
  nfds = 0;
#ifdef __unix
  if (nfds < sockt) nfds = sockt;
#endif

  /* Get the socket that RTCP control information is transmitted on, so we
   *      can select() on it. */
  err = RTPSessionGetRTCPSocket(cid, &sockt);
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }
  sock[1] = sockt;
#ifdef __unix
  if (nfds < sockt) nfds = sockt;
#endif

  return nfds;
}

int init_rtp_player(char *path) {
  if( !path )
    return -1;
  if( (input = open(path, O_RDONLY)) < 0 )
    return -2;
  init_rtp_lib();
}

void rtp_play() {
  if( input < 2 )
    return -1;
  if( !sock[0] || !sock[1] )
    return -2;

  //select(
}

void tcp_seek(int position) {
}

void tcp_pause() {

}

void tcp_stop() {
}
