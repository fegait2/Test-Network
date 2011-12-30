/* Example 1 -- a minimal RTP Sender.
 * Send a file specified on the command line out to the specified
 * RTP port as 8-bit mu-law audio data.
 *
 * Copyright 1997, 1998 Lucent Technologies, all rights reserved.
 *
 * BUGS: If we play a .au file we play out the au header as audio in the
 * first packet.
 */
   
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

#ifdef WIN32
#include <winsock2.h>
#define MAXHOSTNAMELEN  50
extern void gettimeofday(struct timeval *, void *);
#endif

#include "rtp_api.h"
#include "rtp_highlevel.h"

#define PAYLOAD_TYPE_MULAW_8 0

/* Convert a struct timeval to a double */
#define tv2dbl(tv) ((tv).tv_sec + (tv).tv_usec / 1000000.0)

/* Convert a double to a struct timeval */
static struct timeval dbl2tv(double d) 
{
  struct timeval tv;

  tv.tv_sec = (long) d;
  tv.tv_usec = (long) ((d - (long) d) * 1000000.0);
  
  return tv;
}

/* Functions that implement the RTP Scheduler */

/* We maintain a simple queue of events. */
/* If you're not using the library in a simple command-line tool like this,
   you will probably need to tie in to your UI library's event queue
   somehow, instead of using this simple approach.*/
struct evt_queue_elt {
  context cid;
  rtp_opaque_t event_opaque;
  double event_time;
  struct evt_queue_elt *next;
};

static struct evt_queue_elt* evt_queue = NULL;

static void insert_in_evt_queue(struct evt_queue_elt *elt)
{
  if (evt_queue == NULL || elt->event_time < evt_queue->event_time) {
    elt->next = evt_queue;
    evt_queue = elt;
  }
  else {
    struct evt_queue_elt *s = evt_queue;
    while (s != NULL) {
      if (s->next == NULL || elt->event_time < s->next->event_time) {
	elt->next = s->next;
	s->next = elt;
	break;
      }
      s = s->next;
    }
  }
}

/* The library calls this function when it needs to schedule an event.
 *
 * Note that if we were running more than one stream at once we'd need
 * some way of corrolating the event to run with the context ID this
 * function gets passed.
 */
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

/* Parse an addr/port/ttl string. */
static int hpt(char *h, u_int16 *port, unsigned char *ttl)
{
  char *s;

  /* first */
  s = strchr(h, '/');
  if (!s) {
    return -1;
  }
  else {
    *s = '\0';
    *port = atoi(s+1);
    s = strchr(s+1, '/');
    if (s && ttl) {
      *ttl = atoi(s+1);
    }
  }
  return 0;
} /* hpt */


/* Set up an RTP Library connection.
 * Return the maximum file descriptor number used, or -1 on error.
 * remote should contain the address to connect to in
 * host/port[/ttl] format.  (If ttl is not specified, '1' is used.)
 * cid will contain the context ID of the RTP session on sucessful return.
 * sock will contain the file descriptors of the RTP and RTCP ports.
 *
 * BUG: on failure, we should tear down the partially-completed
 * connections. */
static int setup_connection(char *remote, context *cid, int sock[2])
{
  /* Error values that the RTP library returns. */
  rtperror err;
  /* The port of the remote address */
  u_int16 port;
  /* The time-to-live value to set if we're talking to a multicast
   * address. */
  unsigned char ttl = 1;
  /* The sockaddr that hpt returns to us.  Note that we don't actually
   * pass this value to the library; it's just a convenient structure. */
  /* The type the library uses to keep track of sockets. */
  socktype sockt;
  /* The highest file descriptor we've dealt with. */
  int nfds = 0;

  /* Some values for creating the CNAME (see below). */
  char *username;
  char hostname[MAXHOSTNAMELEN];
  char cname[MAXHOSTNAMELEN+32];
  struct timeval tp;
  person uid, conid;
#ifdef WIN32
  WORD wVersionRequested;
  WSADATA wsaData;
  int err2;
 
  wVersionRequested = MAKEWORD( 2, 0 );
 
  err2 = WSAStartup( wVersionRequested, &wsaData );
  if ( err2 != 0 ) {
	  /* Tell the user that we couldn't find a usable */
	  /* WinSock DLL.                                  */
	  return(-1);
  }
#endif
  /* Translate host/port[/ttl] structure into sockaddr_in and ttl value. */
  if (hpt(remote, &port, &ttl) < 0) {
    fprintf(stderr, "%s: bad address\n", remote);
    return -1;
  }

  /* Create the RTP session -- allocate data structures.
   * This doesn't open any sockets. */
  err = RTPCreate(cid);
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }

  /* Set the RTP Session's host address to send to. */
  err = RTPSessionAddSendAddr(*cid, remote, port, ttl);
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }

  /* Set the RTP Session's local receive address */
  /* We set this to the dest for the following reason:
     - It's necessary if the dest is a multicast group, so we join the group
     - It's okay if the dest is unicast, since setting a dest which isn't a
       local address is ignored. */
  err = RTPSessionSetReceiveAddr(*cid, remote, port);


  /* Set up our CNAME (canonical name): should be of the form user@host */
  username = getenv("USER");
  if (gethostname(hostname, MAXHOSTNAMELEN) < 0) {
    perror("gethostname");
    return -1;
  }
  if (username) { sprintf(cname, "%s@%s", username, hostname); }
  else { strcpy(cname, hostname); }

  /* Member 0 is the local member (us) */
  err = RTPMemberInfoSetSDES(*cid, 0, RTP_MI_CNAME, cname);
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }

  /* Set up our NAME (standard display name) */
  err = RTPMemberInfoSetSDES(*cid, 0, RTP_MI_NAME, "RTP Example Server");
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }

  /* Add a single contributor */

  gettimeofday(&tp, NULL);
  srand(tp.tv_usec);
  conid = rand();
  err = RTPSessionAddToContributorList(*cid, (int) conid);
  if (err) {
    fprintf(stderr, "%s %s\n", RTPStrError(err), RTPDebugStr());
    return -1;
  }
  err = RTPSessionGetUniqueIDForCSRC(*cid,conid,&uid);
  if (err) {
    fprintf(stderr, "%s %s\n", RTPStrError(err), RTPDebugStr());
    return -1;
  }
  sprintf(cname, "Contributor %d", (int) conid);
  err = RTPMemberInfoSetSDES(*cid, uid, RTP_MI_CNAME, cname);
  if (err) {
    fprintf(stderr, "%s %s\n", RTPStrError(err), RTPDebugStr());
    return -1;
  }
  err = RTPMemberInfoSetSDES(*cid, uid, RTP_MI_NAME, cname);
  if (err) {
    fprintf(stderr, "%s %s\n", RTPStrError(err), RTPDebugStr());
    return -1;
  }


  /* Open the connection.  We're now live on the network. */
  err = RTPOpenConnection(*cid);
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }

  /* Get the socket that RTP data is transmitted on, so we can select() on
   * it. */
  /* Note that the value passed to it is a socktype *, not an int *. */
  err = RTPSessionGetRTPSocket(*cid, &sockt);
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
     can select() on it. */
  err = RTPSessionGetRTCPSocket(*cid, &sockt);
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

int main(int argc, char *argv[])
{
  /* The context id the RTP library gives us to keep track of our RTP
     stream. */
  context cid;
  /* The two socket fds (RTP and RTCP) we need to select on. */
  socktype sock[2];
  /* The highest file descriptor we've seen, of those we need to select
     on. */
  int nfds;

  /* The audio file we're playing. */
  FILE *input;
  /* Play 20 ms packets outgoing. */
#define BUFFER_SIZE 160
  /* The buffer for our outgoing data. */
  char buffer[BUFFER_SIZE];
  /* Maximum size of a UDP packet. */
#define RECEIVE_BUFFER_SIZE 65536
  /* The buffer incoming data accumulates in.  We don't actually care
   * about the contents of this data, but we must call RTPReceive() so our
   * sender reports and receiver reports are correct. */
  char receive_buffer[RECEIVE_BUFFER_SIZE];

  /* Some values for keeping track of amount of data read from the file */
  int bytes_read, total_read, last_read;
  /* Some values for keeping track of when to send packets. */
  struct timeval start_time_tv, now_tv;
  double start_time, play_time, now;
  /* Information about the media stream we're using. */
  double sample_rate, byte_rate;

  /* The amount to increment the RTP timestamp for a given packet */
  int tsinc;
  /* The RTP marker bit; initially, it's set. */
  char marker = 1;

  /* Error values that the RTP library returns. */
  rtperror err;
 



  /* 8khz 8-bit mulaw */
  sample_rate = byte_rate = 1/8000.0;

  if (argc < 3) {
    fprintf(stderr, "usage: %s file port\n", argv[0]);
    exit(2);
  }

  input = fopen(argv[1], "r");
  if (input == NULL) {
    perror(argv[1]);
    exit(1);
  }

  if ((nfds = setup_connection(argv[2], &cid, sock)) < 0) {
    exit(1);
  }
  
  total_read = 0;
  last_read = BUFFER_SIZE; /* Arbitrary initial setting. */
  gettimeofday(&start_time_tv, NULL);
  start_time = tv2dbl(start_time_tv);

  while((bytes_read = fread(buffer, 1, BUFFER_SIZE, input)) > 0) {
    /* The tsinc (time-stamp increment) is equal to the number of samples
     * played in the *previous* packet -- it's the amount to increment the 
     * RTP timestamp from the previous packet to this one. */
    tsinc = last_read * (sample_rate/byte_rate);
    /* Send the data we just read to the network.
     * We're assuming here that the payload is always Mulaw-8.
     * See comments above for explanations of the other arguments. */
    err = RTPSend(cid, tsinc, marker, PAYLOAD_TYPE_MULAW_8, buffer,
		  bytes_read);
    if (err != RTP_OK) {
      fprintf(stderr, "%s\n", RTPStrError(err));
      exit(1);
    }
    /* The marker bit doesn't get set for any packet but the first. */
    marker = 0;

    total_read += bytes_read;
    last_read = bytes_read;

    /* Schedule the times to play packets as an absolute offset from
     * our start time, rather than a relative offset from the initial
     * packet.  (We're less vulnerable to drifting clocks that way). */
    play_time = start_time + (total_read * byte_rate);

    /* Handle RTP events and received RTP and RTCP packets until the next
     * play time. */
    while (gettimeofday(&now_tv, NULL),
	   (now = tv2dbl(now_tv)) < play_time) {
      int event;
      int retval, i;
      double timeout;
      struct timeval timeout_tv;
      fd_set sockets;

      /* If we have pending events which are coming before the next packet,
       * timeout for them rather than for the next packet to play. */
      if (evt_queue != NULL && evt_queue->event_time < play_time) {
	event = 1;
	timeout = evt_queue->event_time - now;
      }
      else {
	event = 0;
	timeout = play_time - now;
      }
      if (timeout < 0) timeout = 0;
      timeout_tv = dbl2tv(timeout);
      
      FD_ZERO(&sockets);
      FD_SET(sock[0], &sockets);
      FD_SET(sock[1], &sockets);

      retval = select(nfds + 1, &sockets, NULL, NULL, &timeout_tv);
      if (retval < 0) { /* select returned an error */
	perror("select");
	exit(1);
      }
      else if (retval > 0) { /* select reported readable fd's. */
	for (i = 0; i < 2; i++) {
	  if (FD_ISSET(sock[i], &sockets)) {
	    int recbuflen = RECEIVE_BUFFER_SIZE;
	    /* We don't care about the contents of the received packet;
	     * we're only a sender.  However, we still have to call
	     * RTPReceive to ensure that our sender reports and 
	     * receiver reports are correct. */
	    err = RTPReceive(cid, sock[i], receive_buffer, &recbuflen);
	    if (err != RTP_OK && err != RTP_PACKET_LOOPBACK) {
	      fprintf(stderr, "RTPReceive %s (%d): %s\n",
		      i ? "RTCP" : "RTP", sock[i],
		      RTPStrError(err));
	    }
	  }
	}
      }
      else { /* retval == 0, select timed out */
	if (event) {
	  struct evt_queue_elt *next;
	  gettimeofday(&now_tv, NULL);
	  now = tv2dbl(now_tv);
	  while (evt_queue != NULL && evt_queue->event_time <= now) {
	    /* There is a pending RTP event (currently this means there's
	     * an RTCP packet to send), so run it. */
	    RTPExecute(evt_queue->cid, evt_queue->event_opaque);
	    /* Advance the queue */
	    next = evt_queue->next;
	    free(evt_queue);
	    evt_queue = next;
	  }
	}
	else
	  break; /* Time for the next audio packet */
      }
    }
  }

  /* We're done sending now, so close the connection. */
  /* This sends the RTCP BYE packet and closes our sockets. */
  if ((err = RTPCloseConnection(cid, "Goodbye!")) != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
  }

  /* De-allocate all the data structures reserved for the connection.
   * (This would also close the connection, as well, if we hadn't just done
   * it above.) */
  if ((err = RTPDestroy(cid)) != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
  }

  return 0;
}
