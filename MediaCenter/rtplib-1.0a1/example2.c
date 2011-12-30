/* Example 2 -- a minimal RTP Listener.
 * Listen to the specified address, and print all events that occur.
 * 
 * Copyright 1997, 1998 Lucent Technologies, all rights reserved.
 *
 * BUGS: This assumes there is only one outstanding event scheduled
 * by RTPSchedule() at any one time.  This is currently true of the
 * library, but not guaranteed by the API.
 */
   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

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

#endif

#include "rtp_api.h"
#include "rtp_highlevel.h"

/* For windows, this is linked in via the rtp library */
#ifdef WIN32
extern void gettimeofday(struct timeval *, void *);
#endif

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

/* Prints out reception reports */

void PrintReporting(context cid) {

  member_iterator iter;
  person p2;
  memberstatus ms;
  senderstatus sender;
  rtperror err;
  receiver_report_iterator iter2;
  receiver_report rr;
  ntp64 ntpt;
  int32 rtps;

 if(RTPSessionGetMemberList(cid, &iter) != RTP_OK) {
    printf("Detected error in getting member list\n");
  }

 do {

   RTPCurrentMember(cid, &iter, &p2);
   RTPMemberInfoGetStatus(cid, p2, &ms, &sender);

   if(sender != RTP_SENDER_NOT) {
     
     err = RTPSenderInfoGetFirstReceiverReport(cid, p2, &iter2, &rr);
     if((err != RTP_OK) && (err != RTP_END_OF_LIST)) {
       fprintf(stderr,"Error obtaining receiver report\n");
       fprintf(stderr, "%s\n", RTPStrError(err));
       exit(-1);
     }
     
     printf("Person %d:\n",(int) p2);

     while(err == RTP_OK) {
       printf("   Reported by %u\n",(int) rr.reporter);
       printf("     Fraction Lost: %u\n",rr.fraction_lost);
       printf("     Cum lost: %lu\n",rr.cum_lost);
       printf("     Highest SN: %lu\n",rr.highest_seqno);
       printf("     Jitter: %lu\n",rr.jitter);
       printf("     Last SR: %lu\n",rr.last_sr);
       printf("     DLSR: %lu\n",rr.delay_last_sr);

       err = RTPSenderInfoGetNextReceiverReport(cid, p2, &iter2, &rr);
       if((err != RTP_OK) && (err != RTP_END_OF_LIST)) {
	 fprintf(stderr,"Error obtaining receiver report\n");
	 fprintf(stderr, "%s\n", RTPStrError(err));
	 exit(-1);
       }

     }

     err = RTPSenderInfoGetLocalReception(cid, p2, &rr);
     if(err == RTP_OK) {
       printf("   Local observation:\n");
       printf("     Fraction Lost: %u\n",rr.fraction_lost);
       printf("     Cum lost: %lu\n",rr.cum_lost);
       printf("     Highest SN: %lu\n",rr.highest_seqno);
       printf("     Jitter: %lu\n",rr.jitter);
       printf("     Last SR: %lu\n",rr.last_sr);
       printf("     DLSR: %lu\n",rr.delay_last_sr);
     }
     
     printf("   Info from SR:\n");
     RTPMemberInfoGetNTP(cid, p2, &ntpt);
     printf("    NTP Stamp: %lu.%lu\n", ntpt.secs, ntpt.frac);
     RTPMemberInfoGetRTP(cid, p2, &rtps);
     printf("    RTP Stamp: %lu\n", rtps);
     RTPMemberInfoGetPktCount(cid, p2, &rtps);
     printf("    Packet count: %lu\n", rtps);
     RTPMemberInfoGetOctCount(cid, p2, &rtps);
     printf("    Octet count: %lu\n", rtps);
       
   } else {

     printf("Person %d:\n",(int) p2);
     printf("   Not a sender\n");
   }

 } while(RTPNextMember(cid, &iter, &p2) != RTP_END_OF_LIST);

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

/* Functions that implement library callbacks */

/* The library will call this callback (we tell it to, below) whenever some
   change occurs in a member's state. */
static void update_member_callback(context cid, person id_no, rtpflag flags,
				   char *str)
{
  int32 new_ssrc;
  rtperror err;
  err = RTPMemberInfoGetSSRC(cid, id_no, &new_ssrc);
  if (err != RTP_OK){
    fprintf(stderr,
	    "update_member_callback: RTPMemberInfoGetSSRC(%ld, %ld): %s\n",
	    cid, id_no, RTPStrError(err));
    return;
  }
  printf("Member number %ld (SSRC 0x%lx) ", id_no, new_ssrc);
  switch(flags) {
  case RTP_FLAG_NEW_MEMBER:
    printf("joined.\n");
    return;
  case RTP_FLAG_NEW_SENDER:
    printf("was confirmed as a sender.\n");
    return;
  case RTP_FLAG_EXPIRED_MEMBER:
    printf("timed out.\n");
    return;
  case RTP_FLAG_EXPIRED_SENDER:
    printf("timed out as a sender.\n");
    return;
  case RTP_FLAG_MEMBER_LEAVES:
    printf("left: reason \"%s\".\n", str);
    return;
  case RTP_FLAG_OBSERVE_COLLISION:
    printf("collided!\n");
    return;
  case RTP_FLAG_A_CSRC_COLLIDES:
    printf("collided with a csrc member\n");
    return;
  case RTP_FLAG_UNIQUE_ID_REMAP:
    printf("collided and has now returned\n");
    return;
  case RTP_FLAG_MEMBER_ALIVE:
    printf("is alive.\n");
    return;
  case RTP_FLAG_MEMBER_CONFIRMED:
    printf("was confirmed as a group member.\n");
    return;
  case RTP_FLAG_DELETED_PENDING:
    printf("was deleted (having timed out, and never having been confirmed).\n");
    return;
  case RTP_FLAG_DELETED_MEMBER:
    printf("was deleted (having timed out some time ago).\n");
    return;
  case RTP_FLAG_ADDRESS_CHANGES:
    printf("address changed\n");
    return;
  case RTP_FLAG_COLLIDE_WITH_ME:
    printf("collided with local member\n");
    return;
  case RTP_FLAG_PURPORTED_SENDER:
    printf("is purportedly a sender.\n");
    return;
  case RTP_FLAG_DELETED_SENDER:
    printf("is having its sender state destroyed.\n");
    return;
  default:
    printf("had unknown state change %d.\n", flags);
    return;
  }
}


/* The library will call this callback (we tell it to, below) whenever some
   change occurs in in the information about a member. */
static void changed_memberinfo_callback(context cid, person id_no,
					memberinfo info, char* str,
					rtpflag flags)
{
  int32 new_ssrc;
  rtperror err;

  err = RTPMemberInfoGetSSRC(cid, id_no, &new_ssrc);
  if (err != RTP_OK){
    fprintf(stderr,
	    "changed_memberinfo_callback: RTPMemberInfoGetSSRC(%ld, %ld): %s\n",
	    cid, id_no, RTPStrError(err));
    return;
  }
  printf("Member number %ld (SSRC 0x%lx) has ", id_no, new_ssrc);
  
  switch(info) {
  case RTP_MI_CNAME:
    printf("CNAME");
    break;
  case RTP_MI_NAME:
    printf("NAME");
    break;
  case RTP_MI_EMAIL:
    printf("EMAIL");
    break;
  case RTP_MI_PHONE:
    printf("PHONE");
    break;
  case RTP_MI_LOC:
    printf("LOC");
    break;
  case RTP_MI_TOOL:
    printf("TOOL");
    break;
  case RTP_MI_NOTE:
    printf("NOTE");
    break;
  case RTP_MI_PRIV:
    printf("PRIV");
    break;
  case RTP_MI_H323_CADDR:
    printf("H323 CADDR");
    break;
  default:
    printf("unknown attribute %d", info);
    break;
  }

  printf(" \"%s\".\n", str);
  return;
}


/* The library will call this callback (we tell it to, below) whenever two
 * members are observed to have the same CNAME (and thus are actually the same
 * member. */
static void reverting_id_callback(context cid, person canonical,
				  person to_delete,
				  void *userinfo_from_deletee, rtpflag flags)
{
  printf("Member number %ld was the same as %ld, and was merged into it.\n",
	 to_delete, canonical);
  return;
}

/* The library will call this callback (we tell it to, below) whenever
 * a member's RTP or RTCP origination address changes */
static void changed_member_address_callback(context cid,
					    person id_no,
					    char *addr, char *port,
					    int is_rtcp)
{
  int32 new_ssrc;
  rtperror err;
  err = RTPMemberInfoGetSSRC(cid, id_no, &new_ssrc);
  if (err != RTP_OK){
    fprintf(stderr,
	    "changed_member_address_callback: RTPMemberInfoGetSSRC(%ld, %ld): %s\n",
	    cid, id_no, RTPStrError(err));
    return;
  }
  printf("Member number %ld (SSRC 0x%lx) has ", id_no, new_ssrc);
  if (is_rtcp)
    printf("RTCP");
  else
    printf("RTP");

  printf(" source address %s:%s\n", addr, port);
}


/* The library will call this callback (we tell it to, below) whenever
 * there is an error in sending to the specified destination address.
 *
 * Note that errno is valid at the time this callback is called.
 *
 * BUG: when this happens, we ought to (depending on what errno is)
 * stop sending to this address. */
static void send_error_callback(context cid, char *addr, u_int16 port, u_int8 ttl) 
{
  printf("Error sending to address %s:%d (ttl %d): %s.\n",
	 addr, port, ttl, strerror(errno));
  return;
}


/* Functions that implement signal handlers. */
static int finished = 0;

/* We'll call this when we receive any of SIGTERM, SIGHUP, or SIGQUIT.
 * We just set a flag, and let the main select loop bail us out, since
 * the RTP library is not re-entrant. */
static void done(int sig)
{
  finished = 1;
}



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

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 50
#endif

  char hostname[MAXHOSTNAMELEN];
  char cname[MAXHOSTNAMELEN+32];

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

  /* Tell the library to use our callbacks. */
  /* UpdateMember: */
  err = RTPSetUpdateMemberCallBack(*cid, &update_member_callback);
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }
  /* ChangedMemberInfo: */
  err = RTPSetChangedMemberInfoCallBack(*cid, &changed_memberinfo_callback);
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }
  /* RevertingID: */
  err = RTPSetRevertingIDCallBack(*cid, &reverting_id_callback);
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }
  /* ChangedMemberAddress: */
  err = RTPSetChangedMemberAddressCallBack(*cid, &changed_member_address_callback);
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }
  /* SendError: */
  err = RTPSetSendErrorCallBack(*cid, &send_error_callback);
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
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
    return -1;
  }

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
  err = RTPMemberInfoSetSDES(*cid, 0, RTP_MI_NAME, "RTP Example Listener");
  if (err != RTP_OK) {
    fprintf(stderr, "%s\n", RTPStrError(err));
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

  /* Maximum size of a UDP packet. */
#define RECEIVE_BUFFER_SIZE 65536
  /* The buffer incoming data accumulates in.  We don't actually care
   * about the contents of this data, but we must call RTPReceive() so our
   * sender reports and receiver reports are correct. */
  char receive_buffer[RECEIVE_BUFFER_SIZE];

  /* Some values for keeping track of when to send packets. */
  struct timeval now_tv;
  double now;

  /* Error values that the RTP library returns. */
  rtperror err = RTP_OK;
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
  if (argc < 2) {
    fprintf(stderr, "usage: %s address/port/ttl\n", argv[0]);
    exit(2);
  }

  if ((nfds = setup_connection(argv[1], &cid, sock)) < 0) {
    exit(1);
  }

  /* Set up our signal handler. */
  signal(SIGINT, done);
  signal(SIGTERM, done);

#ifdef __unix
  signal(SIGHUP, done);
#endif
  
  while (!finished) {
    int event;
    int retval, i;
    double timeout;
    struct timeval timeout_tv, *timeout_tvp;
    fd_set sockets;

    gettimeofday(&now_tv, NULL);
    now = tv2dbl(now_tv);
    
    /* If we have pending events which are coming before the next packet,
     * timeout for them rather than for the next packet to play. */
    if (evt_queue != NULL) {
      event = 1;
      timeout = evt_queue->event_time - now;
      if (timeout < 0) timeout = 0;
      timeout_tv = dbl2tv(timeout);
      timeout_tvp = &timeout_tv;
    }
    else {
      event = 0;
      timeout_tvp = NULL;
    }
    
    FD_ZERO(&sockets);
    FD_SET(sock[0], &sockets);
    FD_SET(sock[1], &sockets);
    
    retval = select(nfds + 1, &sockets, NULL, NULL, timeout_tvp);
    if (retval < 0) { /* select returned an error */

#ifdef __unix
      if (errno == EINTR) {
	/* We probably got a signal */
	continue;
      }
#endif
      /* Otherwise something's wrong */
      perror("select");
      exit(1);
    }
    else if (retval > 0) { /* select reported readable fd's. */
      for (i = 0; i < 2; i++) {
	if (FD_ISSET(sock[i], &sockets)) {
	  int recbuflen = RECEIVE_BUFFER_SIZE;
	  /* We don't care about the contents of the received packet per se.
	   * However, all of our callbacks, reporting information about other
	   * members, are called by code that occurs underneath RTPReceive.
	   * Plus this makes sure all our reciver reports are correct. */
	  err = RTPReceive(cid, sock[i], receive_buffer, &recbuflen);
	  /* Packet loopback is normal with multicast */
	  if (err != RTP_OK && err != RTP_PACKET_LOOPBACK) {
	    fprintf(stderr, "RTPReceive %s (%d): %s\n",
		    i ? "RTCP" : "RTP", sock[i],
		    RTPStrError(err));
	  }

	  if(i == 1) PrintReporting(cid);
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
	continue; /* This shouldn't happen, so just loop. */
    }
  }

  /* We're done sending now, so close the connection. */
  /* This sends the RTCP BYE packet and closes our sockets. */
  if ((err = RTPCloseConnection(cid,"Goodbye!")) != RTP_OK) {
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
