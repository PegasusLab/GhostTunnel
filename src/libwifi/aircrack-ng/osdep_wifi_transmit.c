#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "./osdep/osdep.h"
#include "osdep_wifi_transmit.h"


int osdep_sockfd_in = -1;
int osdep_sockfd_out = -1;

static struct wif *_wi_in, *_wi_out;

struct devices
{
    int fd_in,  arptype_in;
    int fd_out, arptype_out;
    int fd_rtc;
} dev;

char *osdep_iface_in = NULL;
char *osdep_iface_out = NULL;


int osdep_start(char *interface1, char *interface2)
{

    osdep_iface_in = malloc(strlen(interface1) + 1);
    strcpy(osdep_iface_in, interface1);

	osdep_iface_out = malloc(strlen(interface2) + 1);
    strcpy(osdep_iface_out, interface2);

	/* open the replay interface */
	_wi_out = wi_open(interface2);
	if (!_wi_out){
		printf("open interface %s failed.\n", interface2);
		return 1;
	}

	dev.fd_out = wi_fd(_wi_out);

	if(!strcmp(interface1, interface2)){

		/* open the packet source */
		_wi_in = _wi_out;
		dev.fd_in = dev.fd_out;

		/* XXX */
		dev.arptype_in = dev.arptype_out;
	}
	else{

		/* open the packet source */
		_wi_in = wi_open(interface1);
		if (!_wi_in){
			printf("open interface %s failed.\n", interface1);
			return 1;
		}

		dev.fd_in = wi_fd(_wi_in);
	}

    return 0;
}


int osdep_send_packet(struct packet *pkt)
{
	struct wif *wi = _wi_out; /* XXX globals suck */
	if (wi_write(wi, pkt->data, pkt->len, NULL) == -1) {
		switch (errno) {
		case EAGAIN:
		case ENOBUFS:
			usleep(10000);
			return 0; /* XXX not sure I like this... -sorbo */
		}

		//perror("wi_write()");
		return -1;
	}

	return 0;
}


struct packet osdep_read_packet()
{
	struct wif *wi = _wi_in; /* XXX */
	int rc;
	struct packet pkt;

	do {
	  rc = wi_read(wi, pkt.data, MAX_IEEE_PACKET_SIZE, NULL);
	  if (rc == -1) {
	    //perror("wi_read()");
	    pkt.len = 0;
	    return pkt;
	  }
	} while (rc < 1);

	pkt.len = rc;
	return pkt;
}


void osdep_stop()
{
	if(_wi_in){
		wi_close(_wi_in);
		_wi_in = NULL;
	}

	if(_wi_out){
		wi_close(_wi_out);
		_wi_out = NULL;
	}

	if(osdep_iface_in){
		free(osdep_iface_in);
		osdep_iface_in = NULL;
	}

	if(osdep_iface_out){
		free(osdep_iface_out);
		osdep_iface_out = NULL;
	}
}
